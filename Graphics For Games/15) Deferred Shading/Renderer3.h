#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include "../../nclgl/Light.h"
#include "../../nclgl/OBJMesh.h"

#define LIGHTNUM  8

class Renderer3 : public OGLRenderer
{
public:
	Renderer3(Window &parent);
	virtual ~Renderer3();

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void DrawScene();
	void DrawPointLights();
	void CombineBuffers();


	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* combinateShader;
	Shader* terrainShader;

	SceneMaterial*		 sceneMat;
	DCombineMaterial*    combinateMat;
	DLightMaterial*		 pointLightMat;
	DTerrainMaterial*    dterrainMat;

	Camera* camera;
	float cameraRotation;
	Matrix4 lookAtViewMatrix;

	Mesh* quad;

	HeightMap* heightMap;
	GLuint textures[9];
	float tarrainheight;
	Vector2 terrainSize;

	PointLight* pointLights;
	OBJMesh* sphere;
	float lightRotation;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;

	Matrix4 shadowMatrix;

	void InitTexture();
	void InitBuffer();
	void InitShader();
	void InitMaterial();
	void InitLight();

	void GenerateScreenTexture(GLuint& into, bool depth = false);
};