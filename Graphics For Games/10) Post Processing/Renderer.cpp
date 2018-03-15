
#include "Renderer.h"

Renderer::Renderer(Window &parent)
	:OGLRenderer(parent)
{

	//heightMap = new HeightMap(TEXTUREDIR"terrain.raw"); //OLD

	postpasses = 1;
	camera = new Camera(0.0f, 200.0f, Vector3(0.0f, 500.0f, 0.0f));
	tarrainheight = 15.0f;
	heightMap = new HeightMap(TEXTUREDIR"my_heightmap.png", Vector3(64.0f, tarrainheight, 64.0f));
	tarrainheight = heightMap->GetTerrainHeight();

	quad = Mesh::GenerateQuad();

	rockTex = SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	snowTex = SOIL_load_OGL_texture(TEXTUREDIR"snow.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (rockTex == 0 || rockTex == 0 || snowTex == 0)
		return;

	InitTexture();

	InitShader();

	InitMaterial();

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	InitBuffer();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !reflectionTex[0])
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	init = true;
}
Renderer::~Renderer()
{
	delete sceneShader;
	delete processShader;
	delete terrainShader;

	delete sceneMat;
	delete postProcessMat;
	delete terrainMat;

	delete heightMap;
	delete quad;
	delete camera;

	glDeleteTextures(2, reflectionTex);
	glDeleteTextures(1, &water);
	glDeleteTextures(1, &processFBO);
	glDeleteTextures(1, &bufferDepthTex);
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::InitTexture()
{
	glBindTexture(GL_TEXTURE_2D, rockTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	float ANISOTROPY = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	SetTextureRepeating(rockTex, true);

	glBindTexture(GL_TEXTURE_2D, grassTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ANISOTROPY = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	SetTextureRepeating(grassTex, true);

	glBindTexture(GL_TEXTURE_2D, snowTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ANISOTROPY = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	SetTextureRepeating(snowTex, true);

}
void Renderer::InitBuffer()
{
	// Generate our scene shadowTex texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &reflectionTex[i]);
		glBindTexture(GL_TEXTURE_2D, reflectionTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	}

	glGenFramebuffers(1, &water); // We ’ll render the scene into this
	glGenFramebuffers(1, &processFBO); // And do post processing in this
	glBindFramebuffer(GL_FRAMEBUFFER, water);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, reflectionTex[0], 0);

	//Framebuffer Validation


}

void Renderer::InitShader()
{
	terrainShader = new Shader(SHADERDIR"TerrainVertex.glsl", SHADERDIR"TerrainFragment.glsl");
	if (terrainShader->LinkProgram() == 0)
		return;
	sceneShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (sceneShader->LinkProgram() == 0)
		return;
	processShader = new Shader(SHADERDIR"PostProcess//RadialBlurVertex.glsl", SHADERDIR"PostProcess//RadialBlurFragment.glsl");
	if (processShader->LinkProgram() == 0)
		return;
}

void Renderer::InitMaterial()
{
	sceneMat = new SceneMaterial(sceneShader);
	sceneMat->SetModelMatrix(&modelMatrix);
	sceneMat->SetProjMatrix(&projMatrix);
	sceneMat->SetViewMatrix(&viewMatrix);
	sceneMat->SetTextureMatrix(&textureMatrix);

	postProcessMat = new PostProcessMaterial(processShader);
	postProcessMat->SetModelMatrix(&modelMatrix);
	postProcessMat->SetProjMatrix(&projMatrix);
	postProcessMat->SetViewMatrix(&viewMatrix);
	postProcessMat->SetTextureMatrix(&textureMatrix);

	terrainMat = new TerrainMaterial(terrainShader);
	terrainMat->SetModelMatrix(&modelMatrix);
	terrainMat->SetProjMatrix(&projMatrix);
	terrainMat->SetViewMatrix(&viewMatrix);
	terrainMat->SetTextureMatrix(&textureMatrix);
}

void Renderer::RenderScene()
{
	DrawScene();
	DrawPostProcess();
	PresentScene();
	SwapBuffers();
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	sceneMat->SetCurrentShader(&currentShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();

	quad->SetMatrial(sceneMat);
	sceneMat->SetTexture(reflectionTex[0]);
	quad->Draw();
	glUseProgram(0);

}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	quad->SetMatrial(postProcessMat);
	for (int i = 0; i < postpasses; ++i) {

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex[1], 0);
		postProcessMat->SetTexture(reflectionTex[0]);
		quad->Draw();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex[0], 0);
		postProcessMat->SetTexture(reflectionTex[1]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, water);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex[0], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	terrainMat->SetCurrentShader(&currentShader);
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);

	heightMap->SetMatrial(terrainMat);
	terrainMat->SetGrassTexture(grassTex);
	terrainMat->SetSnowTexture(snowTex);
	terrainMat->SetRockTexture(rockTex);
	terrainMat->SetTerrainHeigh(tarrainheight);
	heightMap->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}