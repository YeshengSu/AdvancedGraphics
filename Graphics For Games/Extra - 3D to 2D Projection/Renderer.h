#pragma once

#include "../../NCLGL/OGLRenderer.h"
#include "../../NCLGL/Mesh.h"
#include "../../NCLGL/camera.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	/*
	Transforms a world space position into screen coordinates
	*/
	Vector3	WorldPosToScreenPos(Vector3 screenPos);

protected:
	Mesh*	triangle;	//A triangle!
	Mesh*	quad;		//A quad?
	Camera* camera;		//I think this is a camera...
};

