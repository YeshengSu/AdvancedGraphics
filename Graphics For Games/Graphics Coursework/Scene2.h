#pragma once
#include "Scene.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/CubeRobot.h"
#include "../../nclgl/CubeNode.h"
#include "../../nclgl/Material.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/Light.h"
#include <algorithm> 
#include "TextMesh.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;
class Scene2 : public Scene
{
public:
	Scene2(Window &parent);
	virtual ~Scene2();

	virtual void UpdateScene(float msec)override;
	virtual void RenderScene(GLuint renderTarget,GLuint depthMap ,GLuint FBO)override;

protected:
	void InitShaderMaterial();
	void InitTexture();
	void InitBuffer();

	void InitQuad();
	void InitRobot();
	void InitMonster();

	void SetRenderOrder(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawShadowMap();
	void DrawNodes();
	void DrawNode(SceneNode *n);
	void DrawUI();

	Shader* sceneShader;
	Shader* soildColorShader;
	Shader* skeletonShader;
	Shader* textureShader;

	SceneMaterial* glassMat;
	TextureMaterial* planeMat;
	TextureMaterial* SkeletonMat;
	SoildColorMaterial* SoildColorMat;

	SceneNode* root;
	Mesh* quad;

	Frustum frameFrustum;

	GLuint stainedGlassTex;
	GLuint hellKnightTex[3];
	GLuint brickTex[3];

	MD5FileData*skeletonData;
	MD5Node*	skeletonNode;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Camera* camera;
	float cameraRotation;
	Light* light[2];
	Vector2 shadowSize;
	GLuint shadowFrameBuffer[2];
	GLuint shadowTex[2];
	Matrix4 shadowMatrix[2];

	MD5Node* controlMonster;
	Vector3 currentLocation;
	Vector3 direction;
	Vector3 des[4];
	float rotation[4];
	float walkSpeed;
	int index;


	string FPS;
	Font*	basicFont;	//A font! a basic one...
	SceneMaterial* fontMat;
	void DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);

};