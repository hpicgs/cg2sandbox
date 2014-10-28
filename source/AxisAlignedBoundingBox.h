
#pragma once

#include <QVector3D>


class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox();
    virtual ~AxisAlignedBoundingBox();

    bool extend(const QVector3D & vertex);

    const QVector3D & center() const;
    float radius() const;

    const QVector3D & llf() const;
    const QVector3D & urb() const;

    bool inside(const QVector3D & vertex) const;
    bool outside(const QVector3D & vertex) const;

protected:
    QVector3D m_llf;
    QVector3D m_urb;

    QVector3D m_center;
    float m_radius;
};
