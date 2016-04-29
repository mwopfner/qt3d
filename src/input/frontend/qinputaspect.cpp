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

#include "qinputaspect.h"
#include "qinputaspect_p.h"
#include "inputhandler_p.h"
#include "keyboarddevice_p.h"
#include "keyboardhandler_p.h"
#include "mousedevice_p.h"
#include "mousehandler_p.h"
#include <Qt3DInput/qkeyboarddevice.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <Qt3DInput/qmousedevice.h>
#include <Qt3DInput/qmousehandler.h>
#include <Qt3DInput/qinputdeviceintegration.h>
#include <Qt3DInput/qinputsettings.h>
#include <Qt3DInput/private/qgenericinputdevice_p.h>
#include <Qt3DInput/private/qinputdeviceintegrationfactory_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>
#include <QDir>
#include <QLibrary>
#include <QLibraryInfo>
#include <QPluginLoader>

#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxisinput.h>
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DInput/qactioninput.h>
#include <Qt3DInput/qinputchord.h>
#include <Qt3DInput/qinputsequence.h>
#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/action_p.h>
#include <Qt3DInput/private/axisinput_p.h>
#include <Qt3DInput/private/axissetting_p.h>
#include <Qt3DInput/private/actioninput_p.h>
#include <Qt3DInput/private/inputchord_p.h>
#include <Qt3DInput/private/inputsequence_p.h>
#include <Qt3DInput/private/logicaldevice_p.h>
#include <Qt3DInput/private/inputbackendnodefunctor_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/updateaxisactionjob_p.h>
#include <Qt3DInput/private/keyboardmousegenericdeviceintegration_p.h>
#include <Qt3DInput/private/genericdevicebackendnode_p.h>
#include <Qt3DInput/private/inputsettings_p.h>
#include <Qt3DInput/private/eventsourcesetterhelper_p.h>

#ifdef HAVE_QGAMEPAD
# include <Qt3DInput/private/qgamepadinput_p.h>
#endif

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {

QInputAspectPrivate::QInputAspectPrivate()
    : QAbstractAspectPrivate()
    , m_inputHandler(new Input::InputHandler())
    , m_keyboardMouseIntegration(new Input::KeyboardMouseGenericDeviceIntegration(m_inputHandler.data()))
{
}

/*!
    \class Qt3DInput::QInputAspect
    \inmodule Qt3DInput
    \since 5.5
*/

QInputAspect::QInputAspect(QObject *parent)
    : QAbstractAspect(*new QInputAspectPrivate, parent)
{
    setObjectName(QStringLiteral("Input Aspect"));
    registerBackendType<QKeyboardDevice>(QBackendNodeMapperPtr(new Input::KeyboardDeviceFunctor(this, d_func()->m_inputHandler.data())));
    registerBackendType<QKeyboardHandler>(QBackendNodeMapperPtr(new Input::KeyboardHandlerFunctor(d_func()->m_inputHandler.data())));
    registerBackendType<QMouseDevice>(QBackendNodeMapperPtr(new Input::MouseDeviceFunctor(this, d_func()->m_inputHandler.data())));
    registerBackendType<QMouseHandler>(QBackendNodeMapperPtr(new Input::MouseHandlerFunctor(d_func()->m_inputHandler.data())));
    registerBackendType<QAxis>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::Axis, Input::AxisManager>(d_func()->m_inputHandler->axisManager())));
    registerBackendType<QAxisInput>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::AxisInput, Input::AxisInputManager>(d_func()->m_inputHandler->axisInputManager())));
    registerBackendType<QAxisSetting>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::AxisSetting, Input::AxisSettingManager>(d_func()->m_inputHandler->axisSettingManager())));
    registerBackendType<Qt3DInput::QAction>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::Action, Input::ActionManager>(d_func()->m_inputHandler->actionManager())));
    registerBackendType<QActionInput>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::ActionInput, Input::ActionInputManager>(d_func()->m_inputHandler->actionInputManager())));
    registerBackendType<QInputChord>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::InputChord, Input::InputChordManager>(d_func()->m_inputHandler->inputChordManager())));
    registerBackendType<QInputSequence>(QBackendNodeMapperPtr(new Input::InputNodeFunctor<Input::InputSequence, Input::InputSequenceManager>(d_func()->m_inputHandler->inputSequenceManager())));
    registerBackendType<QLogicalDevice>(QBackendNodeMapperPtr(new Input::LogicalDeviceNodeFunctor(d_func()->m_inputHandler->logicalDeviceManager())));
    registerBackendType<QGenericInputDevice>(QBackendNodeMapperPtr(new Input::GenericDeviceBackendFunctor(this, d_func()->m_inputHandler.data())));
    registerBackendType<QInputSettings>(QBackendNodeMapperPtr(new Input::InputSettingsFunctor(d_func()->m_inputHandler.data())));

#ifdef HAVE_QGAMEPAD
    registerBackendType<QGamepadInput>(QBackendNodeMapperPtr(new Input::GenericDeviceBackendFunctor(this, d_func()->m_inputHandler.data())));
#endif

    Q_D(QInputAspect);
    // Plugins are QInputDeviceIntegration instances
    d->loadInputDevicePlugins();

    // KeyboardDevice and MouseDevice also provide their own QInputDeviceIntegration
    d->m_inputHandler->addInputDeviceIntegration(d->m_keyboardMouseIntegration.data());
}

void QInputAspectPrivate::loadInputDevicePlugins()
{
    QStringList keys = QInputDeviceIntegrationFactory::keys();
    Q_FOREACH (const QString &key, keys) {
        Qt3DInput::QInputDeviceIntegration *integration = QInputDeviceIntegrationFactory::create(key, QStringList());
        if (integration != nullptr) {
            m_inputHandler->addInputDeviceIntegration(integration);
            // Initialize will allow the InputDeviceIntegration to
            // register their frontend / backend types,
            // create their managers
            // launch a thread to listen to the actual physical device....
            integration->initialize(q_func());
        }
    }
}

// Note: caller is responsible for ownership
QAbstractPhysicalDevice *QInputAspect::createPhysicalDevice(const QString &name)
{
    Q_D(QInputAspect);
    QAbstractPhysicalDevice *device = nullptr;
    Q_FOREACH (Qt3DInput::QInputDeviceIntegration *integration, d->m_inputHandler->inputDeviceIntegrations()) {
        if ((device = integration->createPhysicalDevice(name)) != nullptr)
            break;
    }
    return device;
}

QStringList QInputAspect::availablePhysicalDevices() const
{
    Q_D(const QInputAspect);
    QStringList deviceNamesList;
    const auto deviceIntegrations = d->m_inputHandler->inputDeviceIntegrations();
    for (const QInputDeviceIntegration *integration : deviceIntegrations)
        deviceNamesList += integration->deviceNames();
    return deviceNamesList;
}

QVector<QAspectJobPtr> QInputAspect::jobsToExecute(qint64 time)
{
    Q_UNUSED(time);
    Q_D(QInputAspect);
    QVector<QAspectJobPtr> jobs;

    d->m_inputHandler->updateEventSource();

    jobs.append(d->m_inputHandler->keyboardJobs());
    jobs.append(d->m_inputHandler->mouseJobs());

    Q_FOREACH (QInputDeviceIntegration *integration, d->m_inputHandler->inputDeviceIntegrations())
        jobs += integration->jobsToExecute(time);

    // All the jobs added up until this point are independents
    // but the axis action jobs will be dependent on these
    const QVector<QAspectJobPtr> dependsOnJobs = jobs;

    // Jobs that update Axis/Action (store combined axis/action value)
    Q_FOREACH (Input::HLogicalDevice devHandle, d->m_inputHandler->logicalDeviceManager()->activeDevices()) {
        QAspectJobPtr updateAxisActionJob(new Input::UpdateAxisActionJob(time, d->m_inputHandler.data(), devHandle));
        jobs += updateAxisActionJob;
        Q_FOREACH (const QAspectJobPtr job, dependsOnJobs)
            updateAxisActionJob->addDependency(job);
    }

    return jobs;
}

void QInputAspect::onRegistered()
{
    Q_D(QInputAspect);
    Qt3DCore::QEventFilterService *eventService = d->services()->eventFilterService();
    Q_ASSERT(eventService);

    // Set it on the input handler which will also handle its lifetime
    d->m_inputHandler->eventSourceHelper()->setEventFilterService(eventService);
}

void QInputAspect::onUnregistered()
{
    Q_D(QInputAspect);
    // At this point it is too late to call removeEventFilter as the eventSource (Window)
    // may already be destroyed
    d->m_inputHandler.reset(nullptr);
}

} // namespace Qt3DInput

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("input", QT_PREPEND_NAMESPACE(Qt3DInput), QInputAspect)
