
#include "Renderer.h"
Renderer::Renderer(Window &parent)
	:OGLRenderer(parent)
{
	camera = new Camera(0, 180, Vector3(5000.0f,1000.0f,5000.0f));

	moveSpeed = 0.00005;
	moveFactor = 0;

	// Create terrain
	Vector2 size(32.0f, 32.0f);
	tarrainheight = 15.0f;
	heightMap = new HeightMap(TEXTUREDIR"my_heightmap.png", Vector3(size.x, tarrainheight, size.y));
	tarrainheight = heightMap->GetTerrainHeight();

	// Create Water
	water = new SceneNode(Mesh::GenerateQuad(), Vector4(0.0f, 0.0f, 1.0f, 0.0f));
	Matrix4 waterMatrix = Matrix4::Translation(Vector3(heightMap->GetWidth() * 32.0f *0.5f, 350.0f, heightMap->GetHeight() * 32.0f *0.5f))
		* Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	water->SetTransform(waterMatrix);
	water->SetModelScale(Vector3(10000.0f, 10000.0f, 0.0f));

	InitShader();

	InitTexture();

	InitBuffer();

	InitMaterial();

	quad = Mesh::GenerateQuad();

	// Create light
	light = new ParallelLight(Vector3(), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector3(1000, -1000, -1000));
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}
Renderer::~Renderer()
{
	delete waterShader;
	delete terrainShader;
	delete sceneShader;
	delete skyboxShader;

	delete waterMat;
	delete terrainMat;
	delete skyboxMat;
	delete sunMat;
	delete sceneMat;

	delete heightMap;
	delete camera;


	delete light;
	delete water->GetMesh();
	delete water;
	delete quad;

	glDeleteTextures(6,textures);
	glDeleteTextures(1, &dudvMap);
	glDeleteTextures(1, &dumpMap);
	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &SunTex);
	glDeleteTextures(9, flare);
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	water->Update(msec);
	viewMatrix = camera->BuildViewMatrix();

	//rotate light
	float yaw = 0.005f;
	static_cast<ParallelLight*>(light)->direction = Matrix4::Rotation(yaw*msec, Vector3(0.0f, 1.0f, 0.0f))*static_cast<ParallelLight*>(light)->direction;

	//move water
	moveFactor += moveSpeed * msec;
	if (moveFactor > 1.0f)
		moveFactor = 0.0f;
	waterMat->SetMoveFactor(moveFactor);
}

void Renderer::RenderScene()
{
	DrawScene();
	//DrawPostProcess();
	//PresentScene();
	SwapBuffers();
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

	glGenTextures(1, &reflectionTex);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &refractionTex);
	glBindTexture(GL_TEXTURE_2D, refractionTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &waterReflectionAndRefractionBuffer); // We render the scene into this
	glBindFramebuffer(GL_FRAMEBUFFER, waterReflectionAndRefractionBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, reflectionTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Framebuffer Validation


}

void Renderer::InitTexture()
{

	//create textures
	textures[0] = SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[1] = SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds_NRM.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[2] = SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds_SPEC.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	textures[3] = SOIL_load_OGL_texture(TEXTUREDIR"grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[4] = SOIL_load_OGL_texture(TEXTUREDIR"grass_NRM.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[5] = SOIL_load_OGL_texture(TEXTUREDIR"grass_SPEC.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	textures[6] = SOIL_load_OGL_texture(TEXTUREDIR"snow.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[7] = SOIL_load_OGL_texture(TEXTUREDIR"snow_NRM.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[8] = SOIL_load_OGL_texture(TEXTUREDIR"snow_SPEC.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

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

	dudvMap = SOIL_load_OGL_texture(TEXTUREDIR"waterDUDV.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SetTextureRepeating(dudvMap, true);

	dumpMap = SOIL_load_OGL_texture(TEXTUREDIR"water_NRM.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	glBindTexture(GL_TEXTURE_2D, dumpMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SetTextureRepeating(dumpMap, true);
	
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"skyright.jpg", TEXTUREDIR"skyleft.jpg", TEXTUREDIR"skytop.jpg",
		TEXTUREDIR"skybottom.jpg", TEXTUREDIR"skyback.jpg", TEXTUREDIR"skyfront.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	SunTex = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\sun.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	glBindTexture(GL_TEXTURE_2D, SunTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SetTextureRepeating(SunTex, true);

	flare[0] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex6.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[1] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[2] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex7.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[3] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex5.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[4] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[5] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex9.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[6] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[7] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex4.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	flare[8] = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\tex8.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	for (int i = 0; i < 9; i++)
	{
		if (textures[i] == 0)
		{
			cout << "texture " << i << " is null" << endl;
			return;
		}
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glGenerateMipmap(GL_TEXTURE_2D);
		float ANISOTROPY = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		SetTextureRepeating(textures[i], true);

	}
}

void Renderer::InitShader()
{
	//create shader
	terrainShader = new Shader(SHADERDIR"TerrainVertex.glsl", SHADERDIR"TerrainFragment.glsl");
	if (terrainShader->LinkProgram() == 0)
		return;

	//create shader
	waterShader = new Shader(SHADERDIR"WaterVertex.glsl", SHADERDIR"WaterFragment.glsl");
	if (waterShader->LinkProgram() == 0)
		return;

	sceneShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (sceneShader->LinkProgram() == 0)
		return;

	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	if (skyboxShader->LinkProgram() == 0)
		return;

	
}

void Renderer::InitMaterial()
{

	//set material
	terrainMat = new TerrainMaterial(terrainShader);
	terrainMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	waterMat = new WaterMaterial(waterShader);
	waterMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	sceneMat = new SceneMaterial(sceneShader);
	sceneMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	skyboxMat = new SkyboxMaterial(skyboxShader);
	skyboxMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	sunMat = new SunMaterial(sceneShader);
	sunMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	sceneMat->SetCurrentShader(&currentShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	viewMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(-0.7f,-0.7f,0.0f)) * Matrix4::Scale(Vector3(0.3f,0.3f,0.3f));
	quad->SetMatrial(sceneMat);
	sceneMat->SetTexture(reflectionTex);
	quad->Draw();
	modelMatrix = Matrix4::Translation(Vector3(0.7f, -0.7f, 0.0f)) * Matrix4::Scale(Vector3(0.3f, 0.3f, 0.3f));
	sceneMat->SetTexture(refractionTex);
	quad->Draw();

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	glUseProgram(0);
}

void Renderer::DrawPostProcess()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	//SetCurrentShader(processShader);
	//projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	//viewMatrix.ToIdentity();
	//UpdateShaderMatrices();

	//glDisable(GL_DEPTH_TEST);
	//quad->SetMatrial(postProcessMat);
	//for (int i = 0; i < postpasses; ++i) {

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex[1], 0);
	//	postProcessMat->SetTexture(reflectionTex[0]);
	//	quad->Draw();
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex[0], 0);
	//	postProcessMat->SetTexture(reflectionTex[1]);
	//	quad->Draw();
	//}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glUseProgram(0);
	//glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawScene()
{
	DrawSkybox();
	float distance = 2 * (camera->GetPosition().y - water->GetWorldTransform().values[13]);
	Vector3 cameraPos = camera->GetPosition();

	/*********************************************
					first render
	*********************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, waterReflectionAndRefractionBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	camera->SetPosition(Vector3(cameraPos.x, cameraPos.y - distance, cameraPos.z));
	camera->InvertPitch();
	viewMatrix = camera->BuildViewMatrix();
	terrainMat->SetClipPlane(Vector4(0, 1, 0, -water->GetWorldTransform().values[13]));

	DrawSkybox();
	glEnable(GL_CLIP_DISTANCE0);
	DrawObject();
	glDisable(GL_CLIP_DISTANCE0);

	camera->SetPosition(cameraPos);
	camera->InvertPitch();
	viewMatrix = camera->BuildViewMatrix();

	/*********************************************
					second render
	*********************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, waterRefractionBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	terrainMat->SetClipPlane(Vector4(0, -1, 0, water->GetWorldTransform().values[13]));
	DrawSkybox();
	glEnable(GL_CLIP_DISTANCE0);
	DrawObject();
	glDisable(GL_CLIP_DISTANCE0);
	/*********************************************
					third render
	*********************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	
	DrawSkybox();
	DrawObject();
	DrawWater();
	DrawFlare();
	glUseProgram(0);
	
}

void Renderer::DrawSkybox()
{
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	/*********************************************
						sky box	
	*********************************************/
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	skyboxMat->SetCurrentShader(&currentShader);
	skyboxMat->SetCubeTex(cubeMap);
	quad->SetMatrial(skyboxMat);
	quad->Draw();

	/*********************************************
						sun
	*********************************************/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	Vector3 toSun = ((-static_cast<ParallelLight*>(light)->direction));
	toSun.Normalise();
	Vector3 distance = toSun * 10000;
	distance += camera->GetPosition();
	Matrix4 rotation;
	rotation.values[0] = viewMatrix.values[0];
	rotation.values[1] = viewMatrix.values[4];
	rotation.values[2] = viewMatrix.values[8];

	rotation.values[4] = viewMatrix.values[1];
	rotation.values[5] = viewMatrix.values[5];
	rotation.values[6] = viewMatrix.values[9];

	rotation.values[8] = viewMatrix.values[2];
	rotation.values[9] = viewMatrix.values[6];
	rotation.values[10] = viewMatrix.values[10];
	modelMatrix = Matrix4::Translation(distance)*rotation * Matrix4::Scale(Vector3(1000.0f, 1000.0f, 0.0f));
	
	sunMat->SetCurrentShader(&currentShader);
	sunMat->SetBrightness(1);
	sunMat->SetTexture(SunTex);
	quad->SetMatrial(sunMat);
	quad->Draw();

	//record for flare
	sunPos = modelMatrix * Vector3(0, 0, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glUseProgram(0);

}

void Renderer::DrawObject()
{
	glEnable(GL_DEPTH_TEST);
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	//draw terrian
	modelMatrix.ToIdentity();
	heightMap->SetMatrial(terrainMat);
	terrainMat->SetCurrentShader(&currentShader);
	terrainMat->SetRockTexture(textures[0], textures[1], textures[2]);
	terrainMat->SetGrassTexture(textures[3], textures[4], textures[5]);
	terrainMat->SetSnowTexture(textures[6], textures[7], textures[8]);
	terrainMat->SetTerrainHeight(tarrainheight);
	terrainMat->SetCameraPos(camera->GetPosition());
	terrainMat->SetLightParameter(light->colour, static_cast<ParallelLight*>(light)->direction, 0);
	terrainMat->SetAmbient(Vector4(0.0, 0.0, 0.0, 0.0));
	heightMap->Draw();

	//draw light
	Vector3 offset = Vector3(5000, 1000, 5000);
	DrawDebugLine(DebugDrawMode::DEBUGDRAW_PERSPECTIVE, static_cast<ParallelLight*>(light)->direction + offset, Vector3(0, 0, 0) + offset, Vector3(1.0f, 1.0f, 1.0f));
	DrawDebugLine(DebugDrawMode::DEBUGDRAW_PERSPECTIVE, Vector3(0, 0, 0) + offset, Vector3(1000, 0, 0) + offset, Vector3(1.0f, 0.0f, 0.0f));
	DrawDebugLine(DebugDrawMode::DEBUGDRAW_PERSPECTIVE, Vector3(0, 0, 0) + offset, Vector3(0, 1000, 0) + offset, Vector3(0.0f, 1.0f, 0.0f));
	DrawDebugLine(DebugDrawMode::DEBUGDRAW_PERSPECTIVE, Vector3(0, 0, 0) + offset, Vector3(0, 0, 1000) + offset, Vector3(0.0f, 0.0f, 1.0f));
}

void Renderer::DrawWater()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	//draw water
	modelMatrix = water->GetWorldTransform() * Matrix4::Scale(water->GetModelScale());
	waterMat->SetCurrentShader(&currentShader);
	waterMat->SetReflectionTexture(reflectionTex);
	waterMat->SetRefractionTexture(refractionTex);
	waterMat->SetDudvMap(dudvMap);
	waterMat->SetDumpMap(dumpMap);
	waterMat->SetDepthMapAndPlane(bufferDepthTex, 100000.0f, 1.0f);
	waterMat->SetCameraPos(camera->GetPosition());
	waterMat->SetAmbient(Vector4(0.0, 0.0, 0.0, 0.0));
	waterMat->SetLightParameter(light->colour, static_cast<ParallelLight*>(light)->direction, 0);
	water->GetMesh()->SetMatrial(waterMat);
	water->Draw(*this);
	glDisable(GL_BLEND);

}

void Renderer::DrawFlare()
{

	Vector4 sunProjVec = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f) * camera->BuildViewMatrix() * Vector4(sunPos.x, sunPos.y, sunPos.z,1.0f);
	Vector2 sunScreenCoord = Vector2(sunProjVec.x / sunProjVec.w, sunProjVec.y / sunProjVec.w);
	if (sunProjVec.w <= 0 )
		return;
	else
	{
		//culculate direction and length
		Vector2 toCentre = Vector2(0.0f,0.0f) - Vector2(sunProjVec.x / sunProjVec.w, sunProjVec.y / sunProjVec.w);
		float length = sqrt((toCentre.x*toCentre.x) + (toCentre.y*toCentre.y));

		//start drawing
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		sceneMat->SetCurrentShader(&currentShader);
		quad->SetMatrial(sceneMat);
		space = 0.3;
		brightness = 1.25 - length * 1.5;
		if (brightness > 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			sceneMat->SetBrightness(brightness);
			for (int i = 0; i < 9; i++)//  // 
			{
				//this one coordinate is upsidedown
				Vector3 distance = Vector3(i * space * toCentre.x + sunScreenCoord.x, i * space * toCentre.y - sunScreenCoord.y, 0.01 * i );
				modelMatrix = Matrix4::Translation(distance) * Matrix4::Scale(Vector3(0.6, 0.6, 0.6));
				sceneMat->SetTexture(flare[i]);
				quad->Draw();
			}
			sceneMat->SetBrightness(1);
			viewMatrix = camera->BuildViewMatrix();
			projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
			glUseProgram(0);
			glDisable(GL_BLEND);

		}
		


	}

}
