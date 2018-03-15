
#include "Renderer.h"

Renderer::Renderer(Window &parent)
	:OGLRenderer(parent)
{

	//heightMap = new HeightMap(TEXTUREDIR"terrain.raw"); //OLD
	tarrainheight = 10.0f;
	heightMap = new HeightMap(TEXTUREDIR"my_heightmap.png", Vector3(32.0f, tarrainheight, 32.0f));
	tarrainheight = heightMap->GetTerrainHeight();
	camera = new Camera(0,180,Vector3());

	currentShader = new Shader(SHADERDIR"TerrainVertex.glsl", SHADERDIR"TerrainFragment.glsl");

	if (currentShader->LinkProgram() == 0)
		return;

	terrainMat = new TerrainMaterial(currentShader);
	terrainMat->SetModelMatrix(&modelMatrix);
	terrainMat->SetProjMatrix(&projMatrix);
	terrainMat->SetViewMatrix(&viewMatrix);
	terrainMat->SetTextureMatrix(&textureMatrix);

	rockTex = SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	snowTex = SOIL_load_OGL_texture(TEXTUREDIR"snow.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);


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


	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	init = true;
}
Renderer::~Renderer()
{
	delete heightMap;
	delete camera;
	delete terrainMat;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	heightMap->SetMatrial(terrainMat);
	terrainMat->SetCurrentShader(&currentShader);
	terrainMat->SetCurrentShader(&currentShader);
	terrainMat->SetRockTexture(rockTex);
	terrainMat->SetGrassTexture(grassTex);
	terrainMat->SetSnowTexture(snowTex);
	terrainMat->SetTerrainHeigh(tarrainheight);

	heightMap->Draw();

	glUseProgram(0);
	SwapBuffers();
}

