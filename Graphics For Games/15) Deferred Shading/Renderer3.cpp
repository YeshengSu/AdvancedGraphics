
#include "Renderer3.h"

Renderer3::Renderer3(Window &parent)
	:OGLRenderer(parent)
{
	lightRotation = 0.0f;
	cameraRotation = 0.0f;
	//heightMap = new HeightMap(TEXTUREDIR"terrain.raw"); //OLD

	camera = new Camera(0, 180, Vector3(5000.0f, 2000.0f, 5000.0f));
	tarrainheight = 5.0f;
	terrainSize = Vector2(32.0f, 32.0f);
	heightMap = new HeightMap(TEXTUREDIR"Terrain\\my_heightmap1.png", Vector3(terrainSize.x, tarrainheight, terrainSize.y));
	tarrainheight = heightMap->GetTerrainHeight();

	quad = Mesh::GenerateQuad();
	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj"))
		return;

	InitLight();

	InitTexture();

	InitShader();

	InitMaterial();

	InitBuffer();

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	init = true;
}
Renderer3::~Renderer3()
{
	delete sceneShader;
	delete terrainShader;
	delete combinateShader;
	delete pointLightShader;

	delete sceneMat;
	delete dterrainMat;
	delete combinateMat;
	delete pointLightMat;

	delete heightMap;
	delete quad;
	delete sphere;
	delete camera;

	delete[] pointLights;

	glDeleteTextures(9, textures);
	glDeleteTextures(1,&bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

	currentShader = nullptr;
}

void Renderer3::UpdateScene(float msec)
{
	//auto rotate
	cameraRotation += 0.01f*msec;
	Vector2 centre;
	centre.x = heightMap->GetWidth()*terrainSize.x * 0.5f;
	centre.y = heightMap->GetHeight()*terrainSize.y * 0.5f;
	Vector3 dir = Vector3(5000.0f, 2000.0f, 5000.0f).GetNormal();
	Vector3 currentPoint = Matrix4::Rotation(cameraRotation, Vector3(0, 1.0f, 0))*dir;
	currentPoint = currentPoint*Vector3(8000.0f, 16000.0f, 8000.0f);
	currentPoint += Vector3(centre.x, 0.0f, centre.y);
	Vector3 up = Vector3(0, 1.0f, 0);
	Vector3 target = Vector3(centre.x, 1.0f, centre.y);
	Matrix4 lookatMatrix = Matrix4::BuildViewMatrix(currentPoint, target, up);
	camera->SetPosition(currentPoint);
	viewMatrix = lookatMatrix;
	
	////manually rotate
	//camera->UpdateCamera(msec);
	//viewMatrix = camera->BuildViewMatrix();


	lightRotation = msec * 0.01f;
}

void Renderer3::InitTexture()
{
	//create textures
	textures[0] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\BarrenReds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[1] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\BarrenReds_NRM.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[2] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\BarrenReds_SPEC.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	textures[3] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[4] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\grass_NRM.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[5] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\grass_SPEC.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	textures[6] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\snow.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[7] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\snow_NRM.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[8] = SOIL_load_OGL_texture(TEXTUREDIR"Terrain\\snow_SPEC.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	for (int i = 0; i < 9; i++)
	{
		if (textures[i] == 0)
		{
			cout << "texture " << i << " is null" << endl;
			return;
		}
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		float ANISOTROPY = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		SetTextureRepeating(textures[i], true);

	}

}
void Renderer3::InitBuffer()
{
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	// Generate our scene texture ...
	GenerateScreenTexture(bufferDepthTex,true);
	GenerateScreenTexture(bufferColourTex, false);
	GenerateScreenTexture(bufferNormalTex, false);
	GenerateScreenTexture(lightEmissiveTex, false);
	GenerateScreenTexture(lightSpecularTex, false);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2,buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

}

void Renderer3::InitShader()
{
	terrainShader = new Shader(SHADERDIR"DTerrainVertex.glsl", SHADERDIR"DTerrainFragment.glsl");
	if (terrainShader->LinkProgram() == 0)
		return;
	sceneShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (sceneShader->LinkProgram() == 0)
		return;
	combinateShader = new Shader(SHADERDIR"CombineVertex.glsl", SHADERDIR"CombineFragment.glsl");
	if (combinateShader->LinkProgram() == 0)
		return;
	pointLightShader = new Shader(SHADERDIR"PointLightVertex.glsl", SHADERDIR"PointLightFragment.glsl");
	if (pointLightShader->LinkProgram() == 0)
		return;
}

void Renderer3::InitMaterial()
{
	dterrainMat = new DTerrainMaterial(terrainShader);
	dterrainMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	sceneMat = new SceneMaterial(sceneShader); 
	sceneMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	combinateMat = new DCombineMaterial(combinateShader);
	combinateMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	pointLightMat = new DLightMaterial(pointLightShader);
	pointLightMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
}

void Renderer3::InitLight()
{
	srand(GetTickCount());
	pointLights = new PointLight[LIGHTNUM*LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; x++)
	{
		for (int z = 0; z < LIGHTNUM; z++)
		{
			PointLight& light = pointLights[(x*LIGHTNUM) + z];

			float xPos = ((heightMap->GetWidth()*terrainSize.x) / (LIGHTNUM - 1))*x;
			float zPos = ((heightMap->GetHeight()*terrainSize.y) / (LIGHTNUM - 1))*z;
			light.position = Vector3(xPos,600.0f,zPos);
			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			light.colour = Vector4(r, g, b, 1.0f);

			float radius = ((heightMap->GetWidth()*terrainSize.x) / LIGHTNUM);
			light.radius = radius;

		}
	}
}

void Renderer3::GenerateScreenTexture(GLuint & into, bool depth)
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

void Renderer3::RenderScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawScene();
	DrawPointLights();
	CombineBuffers();
	SwapBuffers();
}

void Renderer3::DrawScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	//draw terrian
	modelMatrix.ToIdentity();
	heightMap->SetMatrial(dterrainMat);
	dterrainMat->SetCurrentShader(&currentShader);
	dterrainMat->SetRockTexture(textures[0], textures[1], textures[2]);
	dterrainMat->SetGrassTexture(textures[3], textures[4], textures[5]);
	dterrainMat->SetSnowTexture(textures[6], textures[7], textures[8]);
	dterrainMat->SetTerrainHeight(tarrainheight);
	dterrainMat->SetShadowMatrix(&shadowMatrix);
	heightMap->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer3::DrawPointLights()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE,GL_ONE);

	pointLightMat->SetDepthTex(bufferDepthTex);
	pointLightMat->SetNormalTex(bufferNormalTex);
	pointLightMat->SetCameraPos(camera->GetPosition());
	pointLightMat->SetPixelSize(Vector2(1.0f / width, 1.0f / height));

	Vector3 translation =
		Vector3((heightMap->GetWidth()*terrainSize.x) / 2.0f, 700.0f, (heightMap->GetHeight()*terrainSize.y));

	Matrix4 pushMatrix = Matrix4::Translation(translation);
	Matrix4 popMatrix = Matrix4::Translation(-translation);

	for (int x = 0; x < LIGHTNUM; x++)
	{
		for (int z = 0; z < LIGHTNUM; z++)
		{
			PointLight& light = pointLights[(x*LIGHTNUM) + z];
			float radius = light.radius;

			modelMatrix =
				pushMatrix* Matrix4::Rotation(lightRotation, Vector3(0, 1, 0))*popMatrix*
				Matrix4::Translation(light.position)*Matrix4::Scale(Vector3(radius, radius, radius));

			light.position = modelMatrix.GetPositionVector();

			pointLightMat->SetLightParameter(light.colour,light.position,light.radius);

			float dist = (light.position - camera->GetPosition()).Length();

			if (dist < radius)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);

			sphere->SetMatrial(pointLightMat);
			sphere->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

}

void Renderer3::CombineBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	combinateMat->SetDiffuseTex(bufferColourTex);
	combinateMat->SetEmissiveTex(lightEmissiveTex);
	combinateMat->SetSpecularTex(lightSpecularTex);

	quad->SetMatrial(combinateMat);
	quad->Draw();

	glUseProgram(0);

}
