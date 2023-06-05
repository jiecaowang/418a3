#pragma once

#include <stack>

#include "Euclidean.h"
#include "Materials.h"
#include "Intersection.h"

// Ray structure. 
class Ray3D {

public:

	Ray3D();
	Ray3D(Point3D p, Vector3D v);

	
	Material* GetTravelingThroughMaterial();

	// Origin and direction of the ray.
	Point3D origin;
	Vector3D dir;
	std::stack<Material*> TraveledThroughMaterial;
	Material* pTravelingThroughMaterial;

	// Intersection status, should be computed by the intersection
	// function.
	Intersection intersection;
	// Current colour of the ray, should be computed by the shading
	// function.
	Colour col;
};
