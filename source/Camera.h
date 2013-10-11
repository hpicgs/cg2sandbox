
#pragma once

#include <QtGlobal>
#include <QObject>

#include <QSize>
#include <QVector3D>
#include <QMatrix4x4>


class Camera : public QObject
{
    Q_OBJECT

public:
    Camera(
        const QVector3D & eye    = QVector3D(0.0, 0.0, 1.0)
    ,   const QVector3D & center = QVector3D(0.0, 0.0, 0.0) 
    ,   const QVector3D & up     = QVector3D(0.0, 1.0, 0.0));

    virtual ~Camera();

    const QVector3D & eye() const;
    void setEye(const QVector3D & eye);
    const QVector3D & center() const;
    void setCenter(const QVector3D & center);
    const QVector3D & up() const;
    void setUp(const QVector3D & up);

    const qreal zNear() const;
    void setZNear(const qreal zNear);
    const qreal zFar() const;
    void setZFar(const qreal zFar);

    const qreal fovy() const;
    void setFovy(const qreal fovy);

    const QSize & viewport() const;
    void setViewport(const QSize & viewport);

    // lazy matrices getters

    const QMatrix4x4 & view();
    const QMatrix4x4 & projection();
    const QMatrix4x4 & viewProjection();

    const QMatrix4x4 & viewInverted();
    const QMatrix4x4 & projectionInverted();
    const QMatrix4x4 & viewProjectionInverted();

    void update();

signals:
    void changed();

protected:
    void dirty(bool update = true);
    void discardInvertedMatrices();

protected:
    bool m_dirty;

    QVector3D m_eye;
    QVector3D m_center;
    QVector3D m_up;

    qreal m_fovy;
    qreal m_aspect;
    qreal m_zNear;
    qreal m_zFar;
    QSize m_viewport;

    QMatrix4x4   m_view;
    QMatrix4x4 * m_viewInverted;
    QMatrix4x4   m_projection;
    QMatrix4x4 * m_projectionInverted;
    QMatrix4x4   m_viewProjection;
    QMatrix4x4 * m_viewProjectionInverted;
};
