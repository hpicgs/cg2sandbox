
#pragma once

class QPointF;
class QMatrix4x4;
class QVector3D;

class AbstractCoordinateProvider
{
public:
    virtual float depthAt(const QPointF & windowCoordinates) = 0;

    virtual const QVector3D objAt(
        const QPointF & windowCoordinates) = 0;

	virtual const QVector3D objAt(
        const QPointF & windowCoordinates
    ,	float depth) = 0;

	virtual const QVector3D objAt(
		const QPointF & windowCoordinates
    ,	float depth
	,	const QMatrix4x4 & viewProjectionInverted) = 0;
};
