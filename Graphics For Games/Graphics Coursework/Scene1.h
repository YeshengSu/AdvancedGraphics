#pragma once
#include "Scene.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include "../../nclgl/Light.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"
#include <algorithm> 
#include "TextMesh.h"
#include <iostream>
#include <sstream>
#include <iomanip>
class Scene1 : public Scene
{
public:
	Scene1(Window &parent);
	virtual ~Scene1();

	virtual void UpdateScene(float msec);
	virtual void RenderScene(GLuint renderTarget, GLuint depthMap, GLuint FBO)override;

protected :
	GLuint textures[9];
	HeightMap* heightMap;
	GLuint shipTex[3];
	Vector2 terrainSize;

	Camera* camera;
	Matrix4 cameraMatrix;
	Matrix4 cameraInverseMatrix;
	float cameraRotation;
	Light* light;

	WaterMaterial* waterMat;
	TerrainMaterial* terrainMat;
	SceneMaterial* sceneMat;
	SkyboxMaterial* skyboxMat;
	SunMaterial* sunMat;
	SoildColorMaterial* soildColorMat;
	StandardMaterial* standardMat;

	Shader* waterShader;
	Shader* terrainShader;
	Shader* sceneShader;
	Shader* skyboxShader;
	Shader* soildColorShader;
	Shader* standardShader;

	Mesh* quad;
	OBJMesh* spaceShip;

	GLuint cubeMap;

	//shadow
	int shadowSize;
	GLuint shadowTex;
	GLuint shadowFBO;
	Matrix4 shadowMatrix;

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
	void DrawShadow();
	void DrawShadowObject();
	void DrawScene();
	void DrawObject();
	void DrawWater();
	void DrawSkybox();
	void DrawFlare();
	void DrawUI();
	void DrawShip();

	float tarrainheight;

	string FPS;
	Font*	basicFont;	//A font! a basic one...
	SceneMaterial* fontMat;
	void DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);
};