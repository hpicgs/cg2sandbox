
#include <QVector3D>
#include <QVector2D>
#include <QPointF>
#include <QQuaternion>
#include <QMatrix4x4>

#include "Camera.h"
#include "FlightNavigation.h"


FlightNavigation::FlightNavigation(Camera * camera)
: m_camera(camera)
, m_mouseSpeed(20.f)
, m_moveSpeed(0.2f)
{
}

FlightNavigation::~FlightNavigation()
{
}

void FlightNavigation::setCamera(Camera * camera)
{
    m_camera = camera;
}

void FlightNavigation::setMouseSpeed(float speed)
{
    m_mouseSpeed = speed;
}

void FlightNavigation::setMoveSpeed(float speed)
{
    m_moveSpeed = speed;
}

const QVector3D FlightNavigation::unproject(const QVector3D & v)
{
    const float w = 2.0f / static_cast<float>(m_camera->viewport().width());
    const float h = 2.0f / static_cast<float>(m_camera->viewport().height());

    const QVector4D p = QVector4D(v.x() * w - 1.f, v.y() * h - 1.f, v.z() - 1.f, 1.f);

    // unproject this point back to object space
    return (m_camera->viewProjectionInverted() * p).toVector3DAffine();
}

void FlightNavigation::mouseMove(const QPointF & mouseDelta)
{
    QVector2D viewport(m_camera->viewport().width(), m_camera->viewport().height());

    float z = m_camera->zFar();

    QVector3D oldScreenCoords(viewport * 0.5f, z);
    QVector3D newScreenCoords(viewport * 0.5f + QVector2D(mouseDelta.x(), -mouseDelta.y()), z);

    QVector3D oldWorldCoords = unproject(oldScreenCoords);
    QVector3D newWorldCoords = unproject(newScreenCoords);

    QVector3D from = oldWorldCoords - m_camera->center();
    QVector3D to = newWorldCoords - m_camera->center();

    QQuaternion rot(QVector3D::dotProduct(from, to), QVector3D::crossProduct(from, to));
    QVector3D eyeDir = (m_camera->center() - m_camera->eye()).normalized();

    QMatrix4x4 R;
    R.rotate(rot);

    eyeDir = R * eyeDir;

    QVector3D delta = eyeDir * m_mouseSpeed;

    m_camera->setCenter(m_camera->eye() + delta);
}

void FlightNavigation::moveLeft()
{
    QVector3D eyeDir(m_camera->center() - m_camera->eye());
    QVector3D sideDir(QVector3D::crossProduct(eyeDir, m_camera->up()));

    move(-sideDir);
}

void FlightNavigation::moveRight()
{
    QVector3D eyeDir(m_camera->center() - m_camera->eye());
    QVector3D sideDir(QVector3D::crossProduct(eyeDir, m_camera->up()));
   
    move(sideDir);
}

void FlightNavigation::moveForward()
{
    QVector3D eyeDir(m_camera->center() - m_camera->eye());

    move(eyeDir);
}

void FlightNavigation::moveBackward()
{
    QVector3D eyeDir(m_camera->center() - m_camera->eye());

    move(-eyeDir);
}

void FlightNavigation::move(const QVector3D & direction)
{
    QVector3D delta(direction.normalized() * m_moveSpeed);

    m_camera->setEye(m_camera->eye() + delta);
    m_camera->setCenter(m_camera->center() + delta);
}
