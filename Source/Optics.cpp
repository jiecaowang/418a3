#include "Optics.h"
#include "Common.h"
#include <cmath>

namespace Optics
{
	Vector3D reflect(Vector3D& incident, Vector3D& normal)
	{
		Vector3D view = -incident;
		Vector3D reflectedRayDir = (2 * (view.dot(normal)) * normal) - view;
		ASSERT(incident.dot(reflectedRayDir) != 0);
		reflectedRayDir.normalize();
		return reflectedRayDir;
	}

	bool isCriticalAngle(Vector3D& incident, Vector3D& normal, double incomingIndex, double outgoingIndex)
	{
		if (outgoingIndex > incomingIndex)
		{
			return false;
		}
		double criticalAngle = asin(outgoingIndex / incomingIndex);
		ASSERT(incident.isNormalized());
		ASSERT(normal.isNormalized());
		double incomingTheta = acos(-1 * incident.dot(normal));
		return (incomingTheta > criticalAngle);
	}

	Vector3D refract(Vector3D& incoming, Vector3D& normal, double incomingIndex, double outgoingIndex)
	{
		ASSERT(incoming.isNormalized());
		ASSERT(normal.isNormalized());

		// Taken from Wikipedia page on snells law
		double r = incomingIndex / outgoingIndex;
		double c = (-normal).dot(incoming);

		Vector3D refractive = r * incoming + (r * c - sqrt(1 - sqr(r) * (1 - sqr(c)))) * normal;
		ASSERT(refractive.normalize());
		return refractive;
	}
}