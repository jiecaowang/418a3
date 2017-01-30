#pragma once

#include "Euclidean.h"
#include "Materials.h"
#include "Intersection.h"

// Ray structure. 
struct Ray3D {
	Ray3D();
	Ray3D(Point3D p, Vector3D v);

	// Origin and direction of the ray.
	Point3D origin;
	Vector3D dir;
	Material* pTravelingThroughMaterial;

	// Intersection status, should be computed by the intersection
	// function.
	Intersection intersection;
	// Current colour of the ray, should be computed by the shading
	// function.
	Colour col;
};
