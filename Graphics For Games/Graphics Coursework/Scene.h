#pragma once
#include "../../nclgl/OGLRenderer.h"

class Scene
{
public :
	Scene(Window *_window)
	{
		window = _window;
		Vector2 size = window->GetScreenSize();
		width = size.x;
		height = size.y;
		renderTarget = 0;
		renderFBO = 0;
		isAutoRotate = true;
	}

	virtual void	RenderScene(GLuint renderTarget,GLuint depthMap, GLuint FBO) = 0;
	virtual void	UpdateScene(float msec) = 0;

	void SetIsAutoRottate(bool isAutoRotate)
	{
		this->isAutoRotate = isAutoRotate;
	}

protected :
	Window* window;

	bool isAutoRotate;

	GLuint renderTarget;
	GLuint renderFBO;
	GLuint renderDepth;

	Matrix4 projMatrix;		//Projection matrix
	Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
	Matrix4 viewMatrix;		//View matrix
	Matrix4 textureMatrix;	//Texture matrix

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
};