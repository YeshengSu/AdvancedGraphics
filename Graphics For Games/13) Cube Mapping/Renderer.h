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
	SkyboxMaterial* skyboxMat;
	SunMaterial* sunMat;

	Shader* waterShader;
	Shader* terrainShader;
	Shader* sceneShader;
	Shader* skyboxShader;

	Mesh* quad;

	GLuint cubeMap;

	//sun and flare
	float space;
	float brightness;
	Vector3 sunPos;
	GLuint SunTex;
	GLuint flare[9];
	Vector2 flarePos[9];

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
	
	void InitBuffer();
	void InitTexture();
	void InitShader();
	void InitMaterial();

	void PresentScene();
	void DrawPostProcess();
	void DrawScene();
	void DrawObject();
	void DrawWater();
	void DrawSkybox();
	void DrawFlare();

	float tarrainheight;
};