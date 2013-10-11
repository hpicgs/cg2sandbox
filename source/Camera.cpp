
#include <cassert>

#include "Camera.h"


Camera::Camera(
    const QVector3D & eye
,   const QVector3D & center
,   const QVector3D & up)
: m_dirty(false)

, m_eye(eye)
, m_center(center)
, m_up(up)

, m_fovy(30.0)
, m_aspect(1.0)
, m_zNear(0.1)
, m_zFar(8.0)
{
}

Camera::~Camera()
{
}

void Camera::discardMatrices()
{
    m_view.invalidate();
    m_viewInverted.invalidate();
    m_projection.invalidate();
    m_viewInverted.invalidate();
    m_projectionInverted.invalidate();
    m_viewProjectionInverted.invalidate();
}

void Camera::dirty(bool update)
{
    m_dirty = true;

    if (update)
        this->update();
}

const QVector3D & Camera::eye() const
{
    return m_eye;
}

void Camera::setEye(const QVector3D & eye)
{
    if (eye == m_eye)
        return;

    m_eye = eye;
    dirty();
}

const QVector3D & Camera::center() const
{
    return m_center;
}

void Camera::setCenter(const QVector3D & center)
{
    if (center == m_center)
        return;

    m_center = center;
    dirty();
}

const QVector3D & Camera::up() const
{
    return m_up;
}

void Camera::setUp(const QVector3D & up)
{
    if (up == m_up)
        return;

    m_up = up;
    dirty();
}

const qreal Camera::zNear() const
{
    return m_zNear;
}

void Camera::setZNear(const qreal zNear)
{
    if (zNear == m_zNear)
        return;

    m_zNear = zNear;
    assert(m_zNear > 0.0);

    dirty();
}

const qreal Camera::zFar() const
{
    return m_zFar;
}

void Camera::setZFar(const qreal zFar)
{
    if (zFar == m_zFar)
        return;

    m_zFar = zFar;
    assert(m_zFar > m_zNear);

    dirty();
}

const qreal Camera::fovy() const
{
    return m_fovy;
}

void Camera::setFovy(const qreal fovy)
{
    if (fovy == m_fovy)
        return;

    m_fovy = fovy;
    assert(m_fovy > 0.0);

    dirty();
}

const QSize & Camera::viewport() const
{
    return m_viewport;
}

void Camera::setViewport(const QSize & viewport)
{
    if (viewport == m_viewport)
        return;

    m_aspect = viewport.width() / qMax<qreal>(static_cast<qreal>(viewport.height()), 1.0);
    m_viewport = viewport;

    dirty();
}

void Camera::update()
{
    if (!m_dirty)
        return;

    discardMatrices();

    m_dirty = false;

    emit(changed());
}

const QMatrix4x4 & Camera::view()
{
    if (m_dirty)
        update();
    
    if (!m_view)
    {
	m_view.value().setToIdentity();
	m_view.value().lookAt(m_eye, m_center, m_up);
	m_view.validate();
    }

    return m_view.value();
}

const QMatrix4x4 & Camera::projection()
{
    if (m_dirty)
        update();
    
    if (!m_projection)
    {
	m_projection.value().setToIdentity();
	m_projection.value().perspective(m_fovy, m_aspect, m_zNear, m_zFar);
	m_projection.validate();
    }

    return m_projection.value();
}

const QMatrix4x4 & Camera::viewProjection()
{
    if (m_dirty)
        update();

    if (!m_viewProjection)
    {
	m_viewProjection.setValue(projection() * view());
    }
    
    return m_viewProjection.value();
}

const QMatrix4x4 & Camera::viewInverted()
{
    if (m_dirty)
        update();

    if (!m_viewInverted)
    {
        m_viewInverted.setValue(QMatrix4x4(view().value().inverted()));
    }

    return m_viewInverted.value();
}

const QMatrix4x4 & Camera::projectionInverted()
{
    if (m_dirty)
        update();

    if (!m_projectionInverted)
    {
        m_projectionInverted.setValue(QMatrix4x4(projection().value().inverted()));
    }

    return m_projectionInverted.value();
}

const QMatrix4x4 & Camera::viewProjectionInverted()
{
    if (m_dirty)
        update();

    if (!m_viewProjectionInverted)
    {
        m_viewProjectionInverted.setValue(QMatrix4x4(viewProjection().value().inverted()));
    }

    return m_viewProjectionInverted.value();
}
