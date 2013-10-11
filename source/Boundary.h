
#pragma once
	
#include <QVector3D>

struct Boundary
{
public:
    Boundary(
        const QVector3D & upperRightBack = QVector3D()
    ,   const QVector3D & lowerLeftFront = QVector3D());

	QVector3D llf;
	QVector3D urb;
};
