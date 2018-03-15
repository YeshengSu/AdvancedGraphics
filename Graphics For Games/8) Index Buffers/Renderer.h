#pragma once

#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected :
	GLuint rockTex;
	GLuint grassTex;
	GLuint snowTex;
	HeightMap* heightMap;
	Camera* camera;
	TerrainMaterial* terrainMat;
	float tarrainheight;
};