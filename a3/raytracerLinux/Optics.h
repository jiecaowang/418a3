#pragma once
#include "Euclidean.h"

namespace Optics
{
	// Reflect a vector3D along a normal
	Vector3D reflect(Vector3D& incident, Vector3D& normal);

	// Return true iff a reflection event occurs during the interaction between two mediums modeled by snells law
	bool isCriticalAngle(Vector3D& incident, Vector3D& normal, double incomingIndex, double outgoingIndex);

	// Refract an incident ray into a medium as governed by snell's law
	Vector3D refract(Vector3D& incoming, Vector3D& normal, double incomingIndex, double outgoingIndex);
}