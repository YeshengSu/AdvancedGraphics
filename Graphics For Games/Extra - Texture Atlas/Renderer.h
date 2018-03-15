#pragma once

#include "./NCLGL/OGLRenderer.h"
#include "./NCLGL/Mesh.h"
#include "./NCLGL/camera.h"

#include "textureAtlas.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void	UpdateAtlasShader(AtlasTreeNode*node);

	Mesh*			quad;
	Camera*			camera;

	//We're going to have a single texture atlas in this example, but if we wanted,
	//we could have an array of them, and dynamically add textures to whichever one
	//we like (we'd have to wrap this around a 'FindAtlasForTexture' function, though)
	TextureAtlas*	atlas;	

	Shader*			basicShader;	//Just the basic textured shader from tutorial 3
	Shader*			atlasShader;	//A slightly fancier shader, that does atlas texture lookups
};

