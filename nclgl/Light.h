#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light
{
public:
	Light(Vector3 position, Vector4 colour)
	{
		this->position = position;
		this->colour = colour;
	}
	~Light(){}

	Vector3 position;
	Vector4 colour;
	float radius;
};

class PointLight : public Light
{
public :
	PointLight(Vector3 position = Vector3(), Vector4 colour = Vector4(),float radius = 1.0f)
		:Light(position,colour)
	{
		this->radius = radius;
	}
	~PointLight(){}

	
};

class ParallelLight : public Light
{
public :
	ParallelLight(Vector3 position = Vector3(), Vector4 colour = Vector4(), Vector3 direction = Vector3())
		:Light(position, colour)
	{
		this->position = direction;
		this->radius = 0.0f;
	}
	~ParallelLight() {}
};