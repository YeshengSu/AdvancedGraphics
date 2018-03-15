#include "Renderer.h"

Renderer::Renderer(Window &parent)
	: OGLRenderer(parent)
{

	quad = Mesh::GenerateQuad();

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	sceneShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (sceneShader->LinkProgram() == 0)
		return;
	sceneMat = new SceneMaterial(sceneShader);
	sceneMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
	quad->SetMatrial(sceneMat);
	SetCurrentShader(sceneShader);

	fontMat = new SceneMaterial(sceneShader);
	fontMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
	fontMat->SetTexture(basicFont->texture);

	glGenFramebuffers(1, &sceneFBO);
	GenerateScreenTexture(renderDepth, true);

	scene1 = new Scene1(parent);
	GenerateScreenTexture(renderTarget1, false);
	scene2 = new Scene2(parent);
	GenerateScreenTexture(renderTarget2, false);
	scene3 = new Scene3(parent);
	GenerateScreenTexture(renderTarget3, false);

	isAutoRotate	= true;
	isRotateKeyUp	= true;
	isLeftKeyUp		= true;
	isRightKeyUp	= true;
	isPauseKeyUp	= true;

	pauseGame = 1;
	selectionScene = SelectedScene::SCENE1;
	init = true;
}
Renderer::~Renderer(void)
{
	delete scene3;
	delete scene2;
	delete scene1;
}

void Renderer::UpdateScene(float msec)
{
	// change camera control mode
	if (KeyUp(isRotateKeyUp, KEYBOARD_T))
	{
		isAutoRotate = !isAutoRotate;
		scene1->SetIsAutoRottate(isAutoRotate);
		scene2->SetIsAutoRottate(isAutoRotate);
		scene3->SetIsAutoRottate(isAutoRotate);
	}

	// change scene
	if (KeyUp(isLeftKeyUp, KEYBOARD_LEFT))
	{
		selectionScene = selectionScene + 1;
		if (selectionScene > 3)
			selectionScene = 0;
	}
	if (KeyUp(isRightKeyUp, KEYBOARD_RIGHT))
	{
		selectionScene = selectionScene - 1;
		if (selectionScene < 0)
			selectionScene = 3;
	}

	//pause game
	if (KeyUp(isPauseKeyUp, KEYBOARD_P))
	{
		pauseGame = pauseGame == 0 ? 1 : 0;
	}

	switch (selectionScene)
	{
	case Renderer::SCENE1:
		scene1->UpdateScene(msec*pauseGame);
		break;
	case Renderer::SCENE2:
		scene2->UpdateScene(msec*pauseGame);
		break;
	case Renderer::SCENE3:
		scene3->UpdateScene(msec*pauseGame);
		break;
	case Renderer::ALL:
		scene1->UpdateScene(msec*pauseGame);
		scene2->UpdateScene(msec*pauseGame);
		scene3->UpdateScene(msec*pauseGame);
		break;
	default:
		break;
	}
	
}

void Renderer::RenderScene()
{
	switch (selectionScene)
	{
	case Renderer::SCENE1:
		scene1->RenderScene(renderTarget1, renderDepth, sceneFBO);
		break;
	case Renderer::SCENE2:
		scene2->RenderScene(renderTarget2, renderDepth, sceneFBO);
		break;
	case Renderer::SCENE3:
		scene3->RenderScene(renderTarget3, renderDepth, sceneFBO);
		break;
	case Renderer::ALL:
		scene1->RenderScene(renderTarget1, renderDepth, sceneFBO);
		scene2->RenderScene(renderTarget2, renderDepth, sceneFBO);
		scene3->RenderScene(renderTarget3, renderDepth, sceneFBO);
		break;
	default:
		break;
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	switch (selectionScene)
	{
	case Renderer::SCENE1:
		modelMatrix.ToIdentity();
		sceneMat->SetTexture(renderTarget1);
		quad->Draw();
		break;
	case Renderer::SCENE2:
		modelMatrix.ToIdentity();
		sceneMat->SetTexture(renderTarget2);
		quad->Draw();
		break;
	case Renderer::SCENE3:
		modelMatrix.ToIdentity();
		sceneMat->SetTexture(renderTarget3);
		quad->Draw();
		break;
	case Renderer::ALL:
		modelMatrix = Matrix4::Translation(Vector3(0.5f, -0.5f, 0.0f))*Matrix4::Scale(Vector3(0.5f, 0.5f, 1.0f));
		sceneMat->SetTexture(renderTarget1);
		quad->Draw();
		modelMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))*Matrix4::Scale(Vector3(0.5f, 0.5f, 1.0f));
		sceneMat->SetTexture(renderTarget2);
		quad->Draw();
		modelMatrix = Matrix4::Translation(Vector3(-0.5f, 0.5f, 0.0f))*Matrix4::Scale(Vector3(0.5f, 0.5f, 1.0f));
		sceneMat->SetTexture(renderTarget3);
		quad->Draw();
		break;
	default:
		break;
	}

	int characterSize = 13;
	float oringinPos = 20.0f;
	float gap = 10.0f;
	DrawText("     switch scene    : left/right arrow",Vector3(0.0f, oringinPos+gap*0,0.5f), characterSize,false);
	DrawText("        Puase        : p", Vector3(0.0f, oringinPos + gap * 1, 0.5f), characterSize, false);
	DrawText("Switch Camera Comtrol: T", Vector3(0.0f, oringinPos + gap * 2, 0.5f), characterSize, false);
	DrawText(" Camera Look Around  : mouse", Vector3(0.0f, oringinPos + gap * 3, 0.5f), characterSize, false);
	DrawText(" Camera Move Comtro  : W A S D", Vector3(0.0f, oringinPos + gap * 4, 0.5f), characterSize, false);
	DrawText("   Camera Up Dowm    : E Q", Vector3(0.0f, oringinPos + gap * 5, 0.5f), characterSize, false);
	SwapBuffers();
}



void Renderer::GenerateScreenTexture(GLuint & into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH24_STENCIL8 : GL_RGBA8,
		width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Renderer::KeyUp(bool & flag, KeyboardKeys key)
{
	if (Window::GetKeyboard()->KeyDown(key))
	{
		if (flag)
		{
			flag = false;
			return true;
		}
	}
	else
	{
		flag = true;
	}
	return false;
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	Matrix4 tempProj = projMatrix;

	mesh->SetMatrial(fontMat);
	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	mesh->Draw();
	glDisable(GL_BLEND);
	projMatrix = tempProj;

	delete mesh; //Once it's drawn, we don't need it anymore!
}