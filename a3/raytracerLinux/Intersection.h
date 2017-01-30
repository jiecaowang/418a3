#pragma once

#include "Euclidean.h"
#include "Materials.h"

struct Intersection {
	Intersection();

	// Location of intersection.
	Point3D point;
	// Normal at the intersection.
	Vector3D normal;
	// Material at the intersection.
	Material* enteringMaterial;

	// Position of the intersection point on your ray.
	// (i.e. point = ray.origin + t_value * ray.dir)
	// This is used when you need to intersect multiply objects and
	// only want to keep the nearest intersection.
	double t_value;
	// Set to true when no intersection has occurred.
	bool none;
};
