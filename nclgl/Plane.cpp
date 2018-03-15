#include "Plane.h"

Plane::Plane(const Vector3 & normal, float distance, bool normalise)
{
	if (normalise)
	{
		float length = sqrt(Vector3::Dot(normal, normal));

		this->normal = normal / length;
		this->distance = distance / length;
	}
	else
	{
		this->normal = normal;
		this->distance = distance;
	}

	return;
}

bool Plane::SphereInPlane(const Vector3 & position, float radius) const
{
	//the direction of normal indicate which side is inside of plane
	//(Vector3::Dot(position, normal) + distance < 0  outside plane
	return ((Vector3::Dot(position, normal) + distance) <= -radius) ? false : true;
	//return true;
}
