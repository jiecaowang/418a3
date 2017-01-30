#include "Ray.h"

Ray3D::Ray3D()
{
	intersection.none = true;
}

Ray3D::Ray3D(Point3D p, Vector3D v) :
	origin(p),
	dir(v)
{
	intersection.none = true;
}