
#pragma once

#include <QVector3D>


class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox();
    virtual ~AxisAlignedBoundingBox();

    const bool extend(const QVector3D & vertex);

    const QVector3D & center() const;
    const float radius() const;

    const QVector3D & llf() const;
    const QVector3D & urb() const;

    const bool inside(const QVector3D & vertex) const;
    const bool outside(const QVector3D & vertex) const;

protected:
    QVector3D m_llf;
    QVector3D m_urb;

    QVector3D m_center;
    float m_radius;
};
