#pragma once

#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include "../../nclgl/Light.h"
#include "../../nclgl/SceneNode.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected :
	GLuint textures[9];
	
	HeightMap* heightMap;

	Camera* camera;
	Light* light;

	WaterMaterial* waterMat;
	TerrainMaterial* terrainMat;
	SceneMaterial* sceneMat;
	
	Shader* waterShader;
	Shader* terrainShader;
	Shader* sceneShader;

	Mesh* quad;

	//water node
	SceneNode* water;
	float moveSpeed;
	float moveFactor;
	GLuint dudvMap;
	GLuint dumpMap;
	GLuint waterReflectionAndRefractionBuffer;
	GLuint waterRefractionBuffer;
	GLuint reflectionTex;
	GLuint refractionTex;
	GLuint bufferDepthTex;
	//
	void InitBuffer();
	void InitTexture();
	void InitShader();

	void PresentScene();
	void DrawPostProcess();
	void DrawScene();
	void RenderObject(bool isRenderWater);
	float tarrainheight;
};