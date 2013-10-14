
#pragma once
	
#include <QVector3D>

struct Boundary
{
public:
    Boundary();
    Boundary(
        const QVector3D & upperRightBack
    ,   const QVector3D & lowerLeftFront);

	QVector3D llf;
	QVector3D urb;
};
