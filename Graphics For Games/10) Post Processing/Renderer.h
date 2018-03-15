#pragma once

#include "../../nclgl/OGLRenderer.h"
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
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	Shader* sceneShader;
	Shader* processShader;
	Shader* terrainShader;

	SceneMaterial*		 sceneMat;
	PostProcessMaterial* postProcessMat;
	TerrainMaterial*     terrainMat;


	Camera* camera;
	Mesh* quad;
	HeightMap* heightMap;



	GLuint rockTex;
	GLuint grassTex;
	GLuint snowTex;
	float tarrainheight;

	GLuint water;
	GLuint processFBO;
	GLuint reflectionTex[2];
	GLuint bufferDepthTex;

	int postpasses;

	void InitTexture();
	void InitBuffer();
	void InitShader();
	void InitMaterial();
};