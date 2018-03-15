#pragma once

#include "../../NCLGL/OGLRenderer.h"
#include "../../NCLGL/Mesh.h"
#include "../../NCLGL/camera.h"
#include "../../NCLGL/SceneNode.h"
#include "../../NCLGL/OBJMesh.h"
#include "Ray.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	Vector3 GetMouseDirectionVector3(float aspect, float fov, Camera &c);

protected:
	void		DrawNode(SceneNode*n);

	Vector3		UnProject(Vector3 position, float aspect, float fov, Camera &c);

	Matrix4		GenerateInverseProjection(float aspect, float fov);
	Matrix4		GenerateInverseView(Camera &c);

	OBJMesh*	cube;
	Camera*		camera;
	Mesh*		triangle;
	SceneNode*	root;
};

