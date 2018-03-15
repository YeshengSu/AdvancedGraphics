#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public :
	Camera(void)
	{
		yaw = 0.0f;
		pitch = 0.0f;
	}

	Camera(float pitch, float yaw, Vector3 position)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}

	~Camera() {}

	void UpdateCamera(float msec = 10.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition()const { return position; }

	void SetPosition(Vector3 val) { position = val; }

	float GetYaw()const { return yaw; }

	void SetYaw(float y) { yaw = y; }

	void InvertYaw() { yaw = -yaw; }

	float GetPitch()const { return pitch; }

	void SetPitch(float p) { pitch = p; }
	
	void InvertPitch() { pitch = -pitch; }

	Vector3 GetUp() { return Matrix4::Rotation(-pitch, Vector3(1.0f, 0.0f, 0.0f)) * Vector3(0.0f, 1.0f, 0.0); }

	Vector3 GetLeft() { return Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(1.0f, 0.0f, 0.0); }
protected :
	float yaw;
	float pitch;
	Vector3 position;
};