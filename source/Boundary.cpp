
#include "Boundary.h"

Boundary::Boundary(
    const QVector3D & upperRightBack
,   const QVector3D & lowerLeftFront)
: urb(upperRightBack)
, llf(lowerLeftFront)
{
}
