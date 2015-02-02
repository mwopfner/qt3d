/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdiffusespecularmapmaterial.h"
#include "qdiffusespecularmapmaterial_p.h"

#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qopenglfilter.h>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QDiffuseSpecularMapMaterialPrivate::QDiffuseSpecularMapMaterialPrivate(QDiffuseSpecularMapMaterial *qq)
    : QMaterialPrivate(qq)
    , m_diffuseSpecularMapEffect(new QEffect())
    , m_diffuseTexture(new QTexture(QTexture::Target2D))
    , m_specularTexture(new QTexture(QTexture::Target2D))
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("diffuseTexture"), m_diffuseTexture))
    , m_specularParameter(new QParameter(QStringLiteral("specularTexture"), m_specularTexture))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
    , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f)))
    , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
    , m_textureScaleParameter(new QParameter(QStringLiteral("texCoordScale"), 1.0f))
    , m_diffuseSpecularMapGL3Technique(new QTechnique())
    , m_diffuseSpecularMapGL2Technique(new QTechnique())
    , m_diffuseSpecularMapES2Technique(new QTechnique())
    , m_diffuseSpecularMapGL3RenderPass(new QRenderPass())
    , m_diffuseSpecularMapGL2RenderPass(new QRenderPass())
    , m_diffuseSpecularMapES2RenderPass(new QRenderPass())
    , m_diffuseSpecularMapGL3Shader(new QShaderProgram())
    , m_diffuseSpecularMapGL2ES2Shader(new QShaderProgram())
{
    m_diffuseTexture->setMagnificationFilter(QTexture::Linear);
    m_diffuseTexture->setMinificationFilter(QTexture::LinearMipMapLinear);
    m_diffuseTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    m_diffuseTexture->setGenerateMipMaps(true);
    m_diffuseTexture->setMaximumAnisotropy(16.0f);

    m_specularTexture->setMagnificationFilter(QTexture::Linear);
    m_specularTexture->setMinificationFilter(QTexture::LinearMipMapLinear);
    m_specularTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    m_specularTexture->setGenerateMipMaps(true);
    m_specularTexture->setMaximumAnisotropy(16.0f);
}

void QDiffuseSpecularMapMaterialPrivate::init()
{
    m_diffuseSpecularMapGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/diffusespecularmap.vert"))));
    m_diffuseSpecularMapGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/diffusespecularmap.frag"))));
    m_diffuseSpecularMapGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/diffusespecularmap.vert"))));
    m_diffuseSpecularMapGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/diffusespecularmap.frag"))));

    m_diffuseSpecularMapGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_diffuseSpecularMapGL3Technique->openGLFilter()->setMajorVersion(3);
    m_diffuseSpecularMapGL3Technique->openGLFilter()->setMinorVersion(1);
    m_diffuseSpecularMapGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_diffuseSpecularMapGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_diffuseSpecularMapGL2Technique->openGLFilter()->setMajorVersion(2);
    m_diffuseSpecularMapGL2Technique->openGLFilter()->setMinorVersion(0);
    m_diffuseSpecularMapGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_diffuseSpecularMapES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_diffuseSpecularMapES2Technique->openGLFilter()->setMajorVersion(2);
    m_diffuseSpecularMapES2Technique->openGLFilter()->setMinorVersion(0);
    m_diffuseSpecularMapES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_diffuseSpecularMapGL3RenderPass->setShaderProgram(m_diffuseSpecularMapGL3Shader);
    m_diffuseSpecularMapGL2RenderPass->setShaderProgram(m_diffuseSpecularMapGL2ES2Shader);
    m_diffuseSpecularMapES2RenderPass->setShaderProgram(m_diffuseSpecularMapGL2ES2Shader);

    m_diffuseSpecularMapGL3Technique->addPass(m_diffuseSpecularMapGL3RenderPass);
    m_diffuseSpecularMapGL2Technique->addPass(m_diffuseSpecularMapGL2RenderPass);
    m_diffuseSpecularMapES2Technique->addPass(m_diffuseSpecularMapES2RenderPass);

    m_diffuseSpecularMapEffect->addTechnique(m_diffuseSpecularMapGL3Technique);
    m_diffuseSpecularMapEffect->addTechnique(m_diffuseSpecularMapGL2Technique);
    m_diffuseSpecularMapEffect->addTechnique(m_diffuseSpecularMapES2Technique);

    m_diffuseSpecularMapEffect->addParameter(m_ambientParameter);
    m_diffuseSpecularMapEffect->addParameter(m_diffuseParameter);
    m_diffuseSpecularMapEffect->addParameter(m_specularParameter);
    m_diffuseSpecularMapEffect->addParameter(m_shininessParameter);
    m_diffuseSpecularMapEffect->addParameter(m_lightPositionParameter);
    m_diffuseSpecularMapEffect->addParameter(m_lightIntensityParameter);
    m_diffuseSpecularMapEffect->addParameter(m_textureScaleParameter);

    static_cast<QMaterial *>(q_ptr)->setEffect(m_diffuseSpecularMapEffect);
}

/*!
    \class Qt3D::QDiffuseSpecularMapMaterial
    \brief The QDiffuseSpecularMapMaterial provides a default implementation of the phong lighting and bump effect where the diffuse and specular light components
    are read from texture maps.
    \inmodule Qt3DRender
    \since 5.5

    The specular lighting effect is based on the combination of 3 lighting components ambient, diffuse and specular.
    The relative strengths of these components is controlled by means of their reflectivity coefficients which are modelled as RGB triplets:

    \list
    \li Ambient is the color that is emitted by an object without any other light source.
    \li Diffuse is the color that is emitted for rought surface reflections with the lights.
    \li Specular is the color emitted for shiny surface reflections with the lights.
    \li The shininess of a surface is controlled by a float property.
    \endlist

    This material uses an effect with a single render pass approach and performs per fragment lighting.
    Techniques are provided for OpenGL 2, OpenGL 3 or above as well as OpenGL ES 2.
*/

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::QDiffuseSpecularMapMaterial()
    Constructs a new Qt3D::QDiffuseSpecularMapMaterial instance with parent object \a parent.
*/
QDiffuseSpecularMapMaterial::QDiffuseSpecularMapMaterial(QNode *parent)
    : QMaterial(*new QDiffuseSpecularMapMaterialPrivate(this), parent)
{
    Q_D(QDiffuseSpecularMapMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    QObject::connect(d->m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));
    d->init();
}

QDiffuseSpecularMapMaterial::~QDiffuseSpecularMapMaterial()
{
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::ambient() const
    \return the current QColor ambient value.
 */
QColor QDiffuseSpecularMapMaterial::ambient() const
{
    Q_D(const QDiffuseSpecularMapMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::diffuse() const
    \return the current diffuse map texture.
    \b {Note:} By default the diffuse texture has a linear
    magnification filter, a linear mipmap linear minification filter,
    the wrap mode is repeat, the maximum anisotropy is set to 16.0
    and mipmapping is enabled.
 */
QTexture *QDiffuseSpecularMapMaterial::diffuse() const
{
    Q_D(const QDiffuseSpecularMapMaterial);
    return d->m_diffuseParameter->value().value<QTexture *>();
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::specular() const
    \return the current specular map texture.
    \b {Note:} By default the specular texture has a linear
    magnification filter, a linear mipmap linear minification filter,
    the wrap mode is repeat, the maximum anisotropy is set to 16.0
    and mipmapping is enabled.
 */
QTexture *QDiffuseSpecularMapMaterial::specular() const
{
    Q_D(const QDiffuseSpecularMapMaterial);
    return d->m_specularParameter->value().value<QTexture *>();
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::shininess() const
    \return the current shininess as a float value.
 */
float QDiffuseSpecularMapMaterial::shininess() const
{
    Q_D(const QDiffuseSpecularMapMaterial);
    return d->m_shininessParameter->value().toFloat();
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::textureScale() const
    \return the current texture scale.
 */
float QDiffuseSpecularMapMaterial::textureScale() const
{
    Q_D(const QDiffuseSpecularMapMaterial);
    return d->m_textureScaleParameter->value().toFloat();
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::setAmbient(const QColor &ambient)
    Sets the current ambient color to \a ambient
 */
void QDiffuseSpecularMapMaterial::setAmbient(const QColor &ambient)
{
    Q_D(QDiffuseSpecularMapMaterial);
    d->m_ambientParameter->setValue(ambient);
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::setDiffuse(QTexture *diffuse)
    Sets the current diffuse map to \a diffuse.
 */
void QDiffuseSpecularMapMaterial::setDiffuse(QTexture *diffuse)
{
    Q_D(QDiffuseSpecularMapMaterial);
    d->m_diffuseParameter->setValue(QVariant::fromValue(diffuse));
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::setSpecular(QTexture *specular)
    Sets the current specular map to \a specular.
 */
void QDiffuseSpecularMapMaterial::setSpecular(QTexture *specular)
{
    Q_D(QDiffuseSpecularMapMaterial);
    d->m_specularParameter->setValue(QVariant::fromValue(specular));
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::setShininess(float shininess)
    Sets the current shininess value to \a shininess.
 */
void QDiffuseSpecularMapMaterial::setShininess(float shininess)
{
    Q_D(QDiffuseSpecularMapMaterial);
    d->m_shininessParameter->setValue(shininess);
}

/*!
    \fn Qt3D::QDiffuseSpecularMapMaterial::setTextureScale(float textureScale)
    Sets the current texture scale to \a textureScale.
 */
void QDiffuseSpecularMapMaterial::setTextureScale(float textureScale)
{
    Q_D(QDiffuseSpecularMapMaterial);
    d->m_textureScaleParameter->setValue(textureScale);
}

} // Qt3D

QT_END_NAMESPACE
