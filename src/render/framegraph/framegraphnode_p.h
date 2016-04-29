/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_FRAMEGRAPHNODE_H
#define QT3DRENDER_RENDER_FRAMEGRAPHNODE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DCore/private/qhandle_p.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/private/managers_p.h>
#include <qglobal.h>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class FrameGraphManager;

class Q_AUTOTEST_EXPORT FrameGraphNode : public BackendNode
{
public:
    FrameGraphNode();
    virtual ~FrameGraphNode();

    enum FrameGraphNodeType {
        InvalidNodeType = 0,
        CameraSelector,
        LayerFilter,
        RenderPassFilter,
        RenderTarget,
        TechniqueFilter,
        Viewport,
        ClearBuffers,
        SortMethod,
        SubtreeSelector,
        StateSet,
        NoDraw,
        FrustumCulling,
        Lighting,
        ComputeDispatch,
        Surface
    };
    FrameGraphNodeType nodeType() const { return m_nodeType; }

    void setFrameGraphManager(FrameGraphManager *manager);
    FrameGraphManager *manager() const;

    void setParentId(Qt3DCore::QNodeId parentId);
    void appendChildId(Qt3DCore::QNodeId childHandle);
    void removeChildId(Qt3DCore::QNodeId childHandle);

    Qt3DCore::QNodeId parentId() const;
    QVector<Qt3DCore::QNodeId> childrenIds() const;

    FrameGraphNode *parent() const;
    QList<FrameGraphNode *> children() const;

protected:
    FrameGraphNode(FrameGraphNodeType nodeType);
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) Q_DECL_OVERRIDE;

private:
    FrameGraphNodeType m_nodeType;
    Qt3DCore::QNodeId m_parentId;
    QVector<Qt3DCore::QNodeId> m_childrenIds;
    FrameGraphManager *m_manager;

    friend class FrameGraphVisitor;
};

template<typename Backend, typename Frontend>
class FrameGraphNodeFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit FrameGraphNodeFunctor(AbstractRenderer *renderer, FrameGraphManager *manager)
        : m_manager(manager)
        , m_renderer(renderer)
    {
    }

    Qt3DCore::QBackendNode *create(Qt3DCore::QNode *frontend) const Q_DECL_OVERRIDE
    {
        return createBackendFrameGraphNode(frontend);
    }

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const Q_DECL_OVERRIDE
    {
        return createBackendFrameGraphNode(change);
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const Q_DECL_OVERRIDE
    {
        return m_manager->lookupNode(id);
    }

    void destroy(Qt3DCore::QNodeId id) const Q_DECL_OVERRIDE
    {
        m_manager->releaseNode(id);
    }

protected:
    Backend *createBackendFrameGraphNode(Qt3DCore::QNode *n) const
    {
        Frontend *f = qobject_cast<Frontend *>(n);
        if (f != nullptr) {
            if (!m_manager->containsNode(n->id())) {
                Backend *backend = new Backend();
                backend->setFrameGraphManager(m_manager);
                backend->setPeer(f);
                backend->setRenderer(m_renderer);
                QFrameGraphNode *parentFGNode = static_cast<QFrameGraphNode *>(n)->parentFrameGraphNode();
                if (parentFGNode)
                    backend->setParentId(parentFGNode->id());
                m_manager->appendNode(backend->peerId(), backend);
                return backend;
            }
            return static_cast<Backend *>(m_manager->lookupNode(n->id()));
        }
        return nullptr;
    }

    Backend *createBackendFrameGraphNode(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
    {
        if (!m_manager->containsNode(change->subjectId())) {
            Backend *backend = new Backend();
            backend->setFrameGraphManager(m_manager);
            backend->setRenderer(m_renderer);
            m_manager->appendNode(change->subjectId(), backend);
            return backend;
        }
        return static_cast<Backend *>(m_manager->lookupNode(change->subjectId()));
    }

private:
    FrameGraphManager *m_manager;
    AbstractRenderer *m_renderer;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FRAMEGRAPHNODE_H
