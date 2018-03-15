#pragma once
#include "Scene3.h"
#include "Scene2.h"
#include "Scene1.h"

class Renderer : public OGLRenderer {
protected :
	enum SelectedScene
	{
		SCENE1,
		SCENE2,
		SCENE3,
		ALL
	};
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void GenerateScreenTexture(GLuint & into, bool depth);

	Shader* sceneShader;
	SceneMaterial*		 sceneMat;

	Mesh* quad;

	GLuint sceneFBO;
	GLuint renderDepth;

	GLuint renderTarget3;
	Scene* scene3;
	GLuint renderTarget2;
	Scene* scene2;
	GLuint renderTarget1;
	Scene* scene1;

	bool isAutoRotate;
	bool isRotateKeyUp;

	int selectionScene;
	bool isLeftKeyUp ;
	bool isRightKeyUp;

	int pauseGame;
	bool isPauseKeyUp;

	bool KeyUp(bool &flag,KeyboardKeys key);

	Font*	basicFont;	//A font! a basic one...
	SceneMaterial* fontMat;
	void DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);
};
