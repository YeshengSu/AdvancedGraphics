#pragma once
#include "Scene.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include "../../nclgl/Light.h"
#include "../../nclgl/OBJMesh.h"
#include "ParticleEmitter.h"
#include "TextMesh.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;
#define LIGHTNUM  8

class Scene3 : public Scene
{
public:
	Scene3(Window &parent);
	virtual ~Scene3();

	virtual void UpdateScene(float msec)override;
	virtual void RenderScene(GLuint renderTarget, GLuint depthMap, GLuint FBO)override;

protected:
	void DrawScene();
	void DrawPointLights();
	void PostProcess();
	void CombineBuffers();
	void DrawUI();

	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* combinateShader;
	Shader* terrainShader;
	Shader* processShader;
	Shader* particleShader;

	SceneMaterial*		 sceneMat;
	DCombineMaterial*    combinateMat;
	DLightMaterial*		 pointLightMat;
	DTerrainMaterial*    dterrainMat;
	PostProcessMaterial* postProcessMat;
	ParticleMaterial*       particleMat;

	ParticleEmitter* particleEmitter;
	GLuint particle;

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

	GLuint processFBO;
	GLuint postProcessTex1;
	GLuint postProcessTex2;

	Matrix4 shadowMatrix;

	void InitTexture();
	void InitBuffer();
	void InitShader();
	void InitMaterial();
	void InitLight();

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	string FPS;
	Font*	basicFont;	//A font! a basic one...
	SceneMaterial* fontMat;
	void DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);
};