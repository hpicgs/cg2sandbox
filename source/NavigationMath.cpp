
#include <cassert>
#include <cfloat>

#include <QVector2D>
#include <QSize>
#include <QMatrix4x4>

#include "MathMacros.h"
#include "Boundary.h"
#include "NavigationMath.h"


bool NavigationMath::validDepth(const float depth)
{
    return depth < (1.0 - std::numeric_limits<float>::epsilon());
}

void NavigationMath::retrieveNearAndFarPlane(
    const QVector3D & camera
,   const QVector3D & center
,   const Boundary & boundary
,   qreal & zNear
,   qreal & zFar)
{
    zNear = 0.01;
    zFar = 1024.0;

    assert(zNear > 0.);
    assert(zFar > zNear);
}

const QVector3D NavigationMath::rayPlaneIntersection(
	bool & valid
,	const QVector3D & rnear
,	const QVector3D & rfar
,	const QVector3D & location
,	const QVector3D & normal)
{
	const QVector3D r0 = rnear; // root of the ray
	const QVector3D &r = rfar - rnear; 

	// intersect with plane in point normal form
	const qreal lDotN = QVector3D::dotProduct(r, normal);

	valid = !r.isNull();

	if(!valid)
		return QVector3D();

	qreal t = QVector3D::dotProduct(location - r0, normal) / lDotN;
	return t * r + r0; // retrieve point via the ray
}

const QVector3D NavigationMath::rayPlaneIntersectionExt(
	const QVector3D & rnear
,	const QVector3D & rfar
,	const QVector3D & location
,	const QVector3D & normal
,   const qreal maxDistance)
{
	bool valid;

	QVector3D i = rayPlaneIntersection(valid, rnear, rfar, location, normal);

    if (valid && i.length() < maxDistance)
		return i;

	const QVector2D r0 = xz(rnear); // root of the ray, projected onto xz-plane
	const QVector2D &r = xz(rfar - rnear);

    i = -rayCircleIntersection(valid, r0, r, maxDistance);
	return QVector3D(i.x(), 0.0, i.y());
}

const QVector2D NavigationMath::raySquareIntersection(
	const QVector2D & point
,	const qreal length)
{
	const qreal ax = abs(point.x());
    const qreal ay = abs(point.y());

	if(ax >= ay) // intersection is with left or right border
        return QVector2D(sign(point.x()), point.y() / ax) * length;
	else // intersection is with bottom or top border
		return QVector2D(point.x() / ay, sign(point.y())) * length;
}

const bool NavigationMath::insideSquare(
	const QVector2D & point
,	const qreal length)
{
	return abs(point.x()) <= length && abs(point.y()) <= length;
}

const QVector2D NavigationMath::rayCircleIntersection(
	bool & valid
,	const QVector2D & origin
,	const QVector2D & ray
,	const qreal radius)
{
	if(ray.isNull())
	{
		valid = false;
		return QVector2D();
	}

	const qreal a = ray.lengthSquared();
	const qreal b = 2. * QVector2D::dotProduct(ray, origin);
    const qreal c = origin.lengthSquared() - static_cast<qreal>(radius * radius);

	const qreal delta = b * b - 4. * a * c;

	if (delta < 0.0) 
	{
		valid = false;
		return QVector2D();
	}

	const qreal s = sqrt(delta);

	// the two intersections
	const qreal t0 = (-b + s) / (2.0 * a);
	const qreal t1 = (-b - s) / (2.0 * a);

	// nearest one
	const qreal t = qMin<qreal>(t0, t1);

	valid = true;
	return t * ray + origin;
}

const QVector2D NavigationMath::xz(const QVector3D & xyz) 
{
	return QVector2D(xyz.x(), xyz.z());
}

const qreal NavigationMath::angle(
	const QVector3D & a
,	const QVector3D & b) 
{
	return acos(QVector3D::dotProduct(a.normalized(), b.normalized()));
}

const bool NavigationMath::boundaryVisible(
    const QMatrix4x4 & mvp
,   const Boundary & b)
{
	const QVector3D box[8] = 
	{
	// front
		QVector3D(b.llf.x(),  b.llf.y(), b.urb.z())
	,	QVector3D(b.urb.x(),  b.llf.y(), b.urb.z())
	,	QVector3D(b.urb.x(),  b.urb.y(), b.urb.z())
	,	QVector3D(b.llf.x(),  b.urb.y(), b.urb.z())
	// back
	,	QVector3D(b.llf.x(),  b.llf.y(), b.llf.z())
	,	QVector3D(b.urb.x(),  b.llf.y(), b.llf.z())
	,	QVector3D(b.urb.x(),  b.urb.y(), b.llf.z())
	,	QVector3D(b.llf.x(),  b.urb.y(), b.llf.z())
	};

	// transform bounds and check if any point is outside NDC (Normalized Device 
    // Coordinates) space 

	for(int i = 0; i < 8; ++i)
	{
		const QVector3D t = mvp * box[i];

		if(qAbs<qreal>(t.x()) > 1.0 || qAbs<qreal>(t.y()) > 1.0)
			return false;
	}
	return true;
}

const bool NavigationMath::pointVisible(
	const QMatrix4x4 & mvp
,	const QVector3D & p)
{
	const QVector3D t = mvp * p;

    return qAbs<qreal>(t.x()) <= 1.0 && qAbs<qreal>(t.y()) <= 1.0;
}

const QVector3D NavigationMath::cameraWithPointInView(
	const QVector3D & eye
,	const QVector3D & center
,	const QVector3D & up
,	const qreal fovy
,	const qreal aspect
,	const QVector3D & point)
{
	const QVector3D ray(center - eye);
	const QVector3D n(ray.normalized());

	// retrieve u and v for orthonormal basis
	const QVector3D v(QVector3D::crossProduct(n, up).normalized());
	const QVector3D u(QVector3D::crossProduct(v,  n).normalized());

	// get both field of view vectors
	const qreal vFov(rad(fovy));
	const qreal hFov(2.0 * atan(tan(vFov * 0.5) * aspect));

	// closest point c
    const QVector3D c = eye + ray * distanceToClosestPoint(eye, center, point);

	// project bc = t to u and v using scalar projection
	const QVector3D t = point - c;

	const qreal uPart = abs(QVector3D::dotProduct(t, u));
	const qreal vPart = abs(QVector3D::dotProduct(t, v)) / aspect;

	// retrieve max distance to camera with required fov for the bigger part
	const qreal p = qMax<qreal>(uPart, vPart);

	// required distance from c to new camera position
	const qreal a = p / tan(vFov * 0.5);

    return c - a * n;
}

const qreal NavigationMath::distanceToClosestPoint(
	const QVector3D & eye
,	const QVector3D & center
,	const QVector3D & point)
{
	const QVector3D ray(center - eye);
	const QVector3D b(point - eye);

	const qreal m = ray.lengthSquared(); // magnitude of ray
	const qreal theta = QVector3D::dotProduct(b, ray);

	if(m == 0.0)
		return 0.0;

    return theta / m; // distance from camera to closest point c;
}
