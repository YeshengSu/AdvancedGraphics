
#include "Scene1.h"

Scene1::Scene1(Window &parent)
	:Scene(&parent)
{
	shadowSize = 2048;

	camera = new Camera(0, 180, Vector3(5000.0f,1000.0f,5000.0f));

	moveSpeed = 0.00005f;
	moveFactor = 0;
	cameraRotation = 0.0;
	InitShader();

	InitTexture();

	InitBuffer();

	InitMaterial();

	// Create terrain
	terrainSize.x = 32.0f;
	terrainSize.y = 32.0f;
	tarrainheight = 15.0f;
	heightMap = new HeightMap(TEXTUREDIR"Terrain\\my_heightmap.png", Vector3(terrainSize.x, tarrainheight, terrainSize.y));
	tarrainheight = heightMap->GetTerrainHeight();

	// Create Water
	water = new SceneNode(Mesh::GenerateQuad(),waterMat, Vector4(0.0f, 0.0f, 1.0f, 0.0f));
	Matrix4 waterMatrix = Matrix4::Translation(Vector3(heightMap->GetWidth() * 32.0f *0.5f, 350.0f, heightMap->GetHeight() * 32.0f *0.5f))
		* Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	water->SetTransform(waterMatrix);
	water->SetModelScale(Vector3(10000.0f, 10000.0f, 0.0f));

	quad = Mesh::GenerateQuad();
	spaceShip = new OBJMesh();
	if (!spaceShip->LoadOBJMesh(MESHDIR"SpaceShip.obj"))
		return;

	// Create light
	light = new ParallelLight(Vector3(), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector3(1000, -1000, -1000));
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);

	isAutoRotate = true;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Scene1::~Scene1()
{
	delete waterShader;
	delete terrainShader;
	delete sceneShader;
	delete skyboxShader;
	delete soildColorShader;
	delete standardShader;

	delete waterMat;
	delete terrainMat;
	delete skyboxMat;
	delete sunMat;
	delete sceneMat;
	delete soildColorMat;
	delete standardMat;
	delete fontMat;

	delete basicFont;
	delete heightMap;
	delete camera;

	delete light;
	delete water->GetMesh();
	delete water;
	delete quad;
	delete spaceShip;

	glDeleteTextures(6,textures);
	glDeleteTextures(1, &dudvMap);
	glDeleteTextures(1, &dumpMap);
	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &SunTex);
	glDeleteTextures(9, flare);
	glDeleteTextures(3, shipTex);
}

void Scene1::UpdateScene(float msec)
{
	//camera comtrol
	if (isAutoRotate)
	{
		//auto rotate
		cameraRotation += 0.01f*msec;
		Vector2 centre;
		centre.x = heightMap->GetWidth()*terrainSize.x * 0.5f;
		centre.y = heightMap->GetHeight()*terrainSize.y * 0.5f;
		Vector3 dir = Vector3(5.0f, 2.0f, 5.0f).GetNormal();
		Vector3 currentPoint = Matrix4::Rotation(cameraRotation, Vector3(0, 1.0f, 0))*dir;
		currentPoint = currentPoint*Vector3(12000.0f, 25000.0f, 12000.0f);
		currentPoint += Vector3(centre.x, 0.0f, centre.y);
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		Vector3 target = Vector3(centre.x, 350.0f, centre.y);
		cameraMatrix = Matrix4::BuildViewMatrix(currentPoint, target, up);
		camera->SetPosition(currentPoint);

		float distance = 2 * (currentPoint.y - water->GetWorldTransform().values[13]);
		Vector3 inversePos = Vector3(currentPoint.x, currentPoint.y - distance, currentPoint.z);
		cameraInverseMatrix = Matrix4::BuildViewMatrix(inversePos, target, up);

		viewMatrix = cameraMatrix;
	}
	else
	{
		////manually rotate
		camera->UpdateCamera(msec);
		cameraMatrix = camera->BuildViewMatrix();

		Vector3 cameraPos = camera->GetPosition();
		float distance = 2 * (cameraPos.y - water->GetWorldTransform().values[13]);
		Vector3 inversePos = Vector3(cameraPos.x, cameraPos.y - distance, cameraPos.z);
		camera->SetPosition(inversePos);
		camera->InvertPitch();
		cameraInverseMatrix = camera->BuildViewMatrix();
		camera->InvertPitch();
		camera->SetPosition(cameraPos);

		viewMatrix = cameraMatrix;
	}

	water->Update(msec);
	//rotate light
	float yaw = 0.0005f;
	//static_cast<ParallelLight*>(light)->position = Matrix4::Rotation(yaw*msec, Vector3(0.0f, 1.0f, 0.0f))*static_cast<ParallelLight*>(light)->position;

	//move water
	moveFactor += moveSpeed * msec;
	if (moveFactor > 1.0f)
		moveFactor = 0.0f;
	waterMat->SetMoveFactor(moveFactor);

	//get fps
	stringstream temp;
	temp << setiosflags(ios::fixed) << setprecision(2) << 1000.0f / msec << endl;
	FPS = temp.str();
}

void Scene1::RenderScene(GLuint _renderTarget, GLuint _depthMap, GLuint _FBO)
{
	renderTarget = _renderTarget;
	renderFBO = _FBO;
	renderDepth = _depthMap;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	DrawScene();
	//DrawPostProcess();
	//PresentScene();
}

void Scene1::InitBuffer()
{
	// Generate our scene shadowTex texture
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	// Generate shadow texture
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);

	// Generate  reflection texture for water
	glGenTextures(1, &reflectionTex);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Generate refraction texture for water
	glGenTextures(1, &refractionTex);
	glBindTexture(GL_TEXTURE_2D, refractionTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// bind shadowTex textuer for buffer
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

void Scene1::InitTexture()
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

	}

	dudvMap = SOIL_load_OGL_texture(TEXTUREDIR"Water\\waterDUDV.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	dumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Water\\water_NRM.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	glBindTexture(GL_TEXTURE_2D, dumpMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"SkyBox\\skyright.jpg", TEXTUREDIR"SkyBox\\skyleft.jpg", TEXTUREDIR"SkyBox\\skytop.jpg",
		TEXTUREDIR"SkyBox\\skybottom.jpg", TEXTUREDIR"SkyBox\\skyback.jpg", TEXTUREDIR"SkyBox\\skyfront.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	SunTex = SOIL_load_OGL_texture(TEXTUREDIR"LensFlare\\sun.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	glBindTexture(GL_TEXTURE_2D, SunTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

	}

	shipTex[0] = SOIL_load_OGL_texture(TEXTUREDIR"Plane\\abc1_tex_d.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	shipTex[1] = SOIL_load_OGL_texture(TEXTUREDIR"Plane\\abc1_tex_d.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	shipTex[2] = SOIL_load_OGL_texture(TEXTUREDIR"Plane\\abc1_tex_s.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	//load image
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

}

void Scene1::InitShader()
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

	soildColorShader = new Shader(SHADERDIR"SoildColorVertex.glsl", SHADERDIR"SoildColorFragment.glsl");
	if (soildColorShader->LinkProgram() == 0)
		return;

	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	if (skyboxShader->LinkProgram() == 0)
		return;

	standardShader = new Shader(SHADERDIR"StandardVertex.glsl", SHADERDIR"StandardFragment.glsl");
	if (standardShader->LinkProgram() == 0)
		return;
	
}

void Scene1::InitMaterial()
{
	//set material
	terrainMat = new TerrainMaterial(terrainShader);
	terrainMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
	terrainMat->SetShadowMatrix(&shadowMatrix);

	waterMat = new WaterMaterial(waterShader);
	waterMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	sceneMat = new SceneMaterial(sceneShader);
	sceneMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
	fontMat = new SceneMaterial(sceneShader);
	fontMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
	fontMat->SetTexture(basicFont->texture);

	skyboxMat = new SkyboxMaterial(skyboxShader);
	skyboxMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	sunMat = new SunMaterial(sceneShader);
	sunMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	soildColorMat = new SoildColorMaterial(soildColorShader);
	soildColorMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	standardMat = new StandardMaterial(standardShader);
	standardMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);
}

void Scene1::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	sceneMat->SetBrightness(1);
	viewMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(-0.7f,-0.7f,0.0f)) * Matrix4::Scale(Vector3(0.3f,0.3f,0.3f));
	quad->SetMatrial(sceneMat);
	sceneMat->SetTexture(reflectionTex);
	//quad->Draw();
	modelMatrix = Matrix4::Translation(Vector3(0.7f, -0.7f, 0.0f)) * Matrix4::Scale(Vector3(0.3f, 0.3f, 0.3f));
	sceneMat->SetTexture(refractionTex);
	//quad->Draw();

	viewMatrix = cameraMatrix;
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	glUseProgram(0);
}

void Scene1::DrawPostProcess()
{

}

void Scene1::DrawShadow()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowSize, shadowSize);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//SetCurrentShader(soildColorShader);

	Vector3 dir = (-static_cast<ParallelLight*>(light)->position);
	dir.Normalise();
	//projMatrix = Matrix4::Perspective(5000.0f, 100000.0f, (float)width / (float)height, 45.0f);
	projMatrix = Matrix4::Orthographic(100, 1000000, 10000, -10000, -10000, 10000);
	viewMatrix = Matrix4::BuildViewMatrix((dir * 20000.0f) + (Vector3(8192.0f, 0.0f, 8192.0f)), Vector3(8192.0f, 0.0f, 8192.0f));
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);
	DrawShadowObject();

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene1::DrawShadowObject()
{
	glEnable(GL_DEPTH_TEST);
	//draw terrian
	modelMatrix.ToIdentity();
	heightMap->SetMatrial(soildColorMat);
	soildColorMat->SetSoildColor(Vector4(0.0f,0.0f,1.0f,1.0f));
	modelMatrix.ToIdentity();
	heightMap->Draw();
	modelMatrix = Matrix4::Translation(Vector3(11500.0f, 900.0f, 5000.0f))*Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f))*Matrix4::Rotation(-20, Vector3(0.0f, 1.0f, 0.0f));
	spaceShip->SetMatrial(soildColorMat);
	spaceShip->Draw();
}

void Scene1::DrawScene()
{
	DrawShadow();

	/*********************************************
					first render
	*********************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, waterReflectionAndRefractionBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	viewMatrix = cameraInverseMatrix;
	terrainMat->SetClipPlane(Vector4(0, 1, 0, -water->GetWorldTransform().values[13]));

	DrawSkybox();
	DrawShip();
	glEnable(GL_CLIP_DISTANCE0);
	DrawObject();
	glDisable(GL_CLIP_DISTANCE0);

	/*********************************************
					second render
	*********************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, waterReflectionAndRefractionBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	viewMatrix = cameraMatrix;
	terrainMat->SetClipPlane(Vector4(0, -1, 0, water->GetWorldTransform().values[13]));
	DrawSkybox();
	glEnable(GL_CLIP_DISTANCE0);
	DrawObject();
	glDisable(GL_CLIP_DISTANCE0);

	/*********************************************
					third render
	*********************************************/
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, renderTarget, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, renderDepth, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	DrawSkybox();
	DrawObject();
	DrawWater();
	DrawShip();
	DrawFlare();
	DrawUI();
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void Scene1::DrawSkybox()
{
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	/*********************************************
						sky box	
	*********************************************/
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	skyboxMat->SetCubeTex(cubeMap);
	quad->SetMatrial(skyboxMat);
	quad->Draw();

	/*********************************************
						sun
	*********************************************/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	Vector3 toSun = ((-static_cast<ParallelLight*>(light)->position));
	toSun.Normalise();
	Vector3 distance = toSun * 10000;
	distance += camera->GetPosition();
	Matrix4 rotation;
	rotation.values[0] = cameraMatrix.values[0];
	rotation.values[1] = cameraMatrix.values[4];
	rotation.values[2] = cameraMatrix.values[8];
						
	rotation.values[4] = cameraMatrix.values[1];
	rotation.values[5] = cameraMatrix.values[5];
	rotation.values[6] = cameraMatrix.values[9];
					
	rotation.values[8] = cameraMatrix.values[2];
	rotation.values[9] = cameraMatrix.values[6];
	rotation.values[10]= cameraMatrix.values[10];
	modelMatrix = Matrix4::Translation(distance)*rotation * Matrix4::Scale(Vector3(1000.0f, 1000.0f, 0.0f));
	
	sunMat->SetBrightness(1);
	sunMat->SetTexture(SunTex);
	quad->SetMatrial(sunMat);
	quad->Draw();

	//record for flare
	sunPos = modelMatrix * Vector3(0, 0, 0);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glUseProgram(0);

}

void Scene1::DrawObject()
{
	glEnable(GL_DEPTH_TEST);

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	//draw terrian
	modelMatrix.ToIdentity();
	heightMap->SetMatrial(terrainMat);
	terrainMat->SetShadowMatrix(&shadowMatrix);
	terrainMat->SetRockTexture(textures[0], textures[1], textures[2]);
	terrainMat->SetGrassTexture(textures[3], textures[4], textures[5]);
	terrainMat->SetSnowTexture(textures[6], textures[7], textures[8]);
	terrainMat->SetTerrainHeight(tarrainheight);
	terrainMat->SetCameraPos(camera->GetPosition());
	terrainMat->SetShadowTex(shadowTex);
	terrainMat->SetLightParameter(light->colour, static_cast<ParallelLight*>(light)->position, 0.0f);
	terrainMat->SetAmbient(Vector4(0.0, 0.0, 0.0, 0.0));
	heightMap->Draw();
}

void Scene1::DrawWater()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	//draw water
	modelMatrix = water->GetWorldTransform() * Matrix4::Scale(water->GetModelScale());
	waterMat->SetReflectionTexture(reflectionTex);
	waterMat->SetRefractionTexture(refractionTex);
	waterMat->SetDudvMap(dudvMap);
	waterMat->SetDumpMap(dumpMap);
	waterMat->SetDepthMapAndPlane(bufferDepthTex, 100000.0f, 1.0f);
	waterMat->SetCameraPos(camera->GetPosition());
	waterMat->SetAmbient(Vector4(0.0, 0.0, 0.0, 0.0));
	waterMat->SetLightParameter(light->colour, static_cast<ParallelLight*>(light)->position, 0);
	water->GetMesh()->SetMatrial(waterMat);
	water->Draw();
	glDisable(GL_BLEND);

}

void Scene1::DrawFlare()
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
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
		quad->SetMatrial(sceneMat);
		space = 0.3f;
		brightness = 1.25f - length * 1.5f;
		if (brightness > 0)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			sceneMat->SetBrightness(brightness);
			for (int i = 0; i < 9; i++)//  // 
			{
				//this one coordinate is upsidedown
				Vector3 distance = Vector3(i * space * toCentre.x + sunScreenCoord.x, i * space * toCentre.y - sunScreenCoord.y, 0.01f * i );
				modelMatrix = Matrix4::Translation(distance) * Matrix4::Scale(Vector3(0.6f, 0.6f, 0.6f));
				sceneMat->SetTexture(flare[i]);
				quad->Draw();
			}
			viewMatrix = cameraMatrix;
			sceneMat->SetBrightness(1);
			projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
			glUseProgram(0);
			glDisable(GL_BLEND);

		}
		


	}

}

void Scene1::DrawUI()
{
	DrawText(FPS, Vector3(0, 0, 0.5f), 20.0f, false);
}

void Scene1::DrawShip()
{
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	modelMatrix= Matrix4::Translation(Vector3(11500.0f, 900.0f, 5000.0f))*Matrix4::Scale(Vector3(50.0f,50.0f,50.0f))*Matrix4::Rotation(-20,Vector3(0.0f,1.0f,0.0f));
	spaceShip->SetMatrial(standardMat);
	standardMat->SetDiffuseTex(shipTex[0]);
	standardMat->SetBumpTex(shipTex[1]);
	standardMat->SetSpecularTex(shipTex[2]);
	standardMat->SetCameraPos(camera->GetPosition());
	standardMat->SetCubeTex(cubeMap,0.6f);
	standardMat->SetShadowTex(shadowTex,1.5f);
	standardMat->SetLightParameter(light->colour, static_cast<ParallelLight*>(light)->position, 0.0f);
	standardMat->SetAmbient(Vector4(0.0, 0.0, 0.0, 0.0));
	spaceShip->Draw();
	modelMatrix.ToIdentity();
}

/*
Draw a line of text on screen. If we were to have a 'static' line of text, we'd
probably want to keep the TextMesh around to save processing it every frame,
but for a simple demonstration, this is fine...
*/
void Scene1::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	Matrix4 tempProj = projMatrix;

	mesh->SetMatrial(fontMat);
	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = cameraMatrix;
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
