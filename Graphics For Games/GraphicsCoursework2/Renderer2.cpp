
#include "Renderer2.h"

Renderer2::Renderer2(Window &parent)
	:OGLRenderer(parent)
{
	CubeRobot::CreateCube();

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	
	camera = new Camera();
	camera->SetPosition(Vector3(0, 550, 800));
	camera->SetPitch(-15);
	cameraRotation = 0.0;

	shadowSize.x = width*2;
	shadowSize.y = height*2;
	light[0] = new ParallelLight(Vector3(700.0f,-500.0f,200.0f),Vector4(1.0f,1.0f,1.0f,1.0f), Vector3(700.0f, -500.0f, 200.0f));
	light[1] = new PointLight(Vector3(600.0f,100.0f,0.0f), Vector4(0.25f, 0.25f, 1.0f, 1.0f),1000);
	InitBuffer();

	InitTexture();

	InitShaderMaterial();
	
	root = new SceneNode();

	InitQuad();

	//InitRobot();

	InitMonster();

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

void Renderer2::InitShaderMaterial()
{
	sceneShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (sceneShader->LinkProgram() == 0)
		return;
	glassMat = new SceneMaterial(sceneShader);
	glassMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	skeletonShader = new Shader(SHADERDIR"skeletonVertex.glsl", SHADERDIR"TextureFragment.glsl");
	if (skeletonShader->LinkProgram() == 0)
		return;
	SkeletonMat = new TextureMaterial(skeletonShader);
	SkeletonMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	textureShader = new Shader(SHADERDIR"TextureVertex.glsl", SHADERDIR"TextureFragment.glsl");
	if (textureShader->LinkProgram() == 0)
		return;
	planeMat = new TextureMaterial(textureShader);
	planeMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	soildColorShader = new Shader(SHADERDIR"SoildColorVertex.glsl", SHADERDIR"SoildColorFragment.glsl");
	if (soildColorShader->LinkProgram() == 0)
		return;
	SoildColorMat = new SoildColorMaterial(soildColorShader);
	SoildColorMat->SetMatrix(&projMatrix, &modelMatrix, &viewMatrix, &textureMatrix);

	currentShader = skeletonShader;
}

void Renderer2::InitTexture() 
{
	stainedGlassTex = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (stainedGlassTex == 0)
		return;
	brickTex[0] = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	brickTex[1] = SOIL_load_OGL_texture(TEXTUREDIR"brick_NRM.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	brickTex[2] = SOIL_load_OGL_texture(TEXTUREDIR"brick_SPEC.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	hellKnightTex[0] = SOIL_load_OGL_texture(TEXTUREDIR"hellknight.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	hellKnightTex[1] = SOIL_load_OGL_texture(TEXTUREDIR"hellknight_NRM.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	hellKnightTex[2] = SOIL_load_OGL_texture(TEXTUREDIR"hellknight_SPEC.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	for (int i = 0; i < 3; i++)
	{
		glBindTexture(GL_TEXTURE_2D, brickTex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		SetTextureRepeating(brickTex[i], true);
	}
}

void Renderer2::InitBuffer()
{
	for (int i = 0; i < 2; i++)
	{
		glGenTextures(1, &shadowTex[i]);
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize.x, shadowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glGenFramebuffers(1, &shadowFrameBuffer[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex[i], 0);
		glDrawBuffer(GL_NONE);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer2::RenderScene()
{
	SetRenderOrder(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawShadowMap();
	DrawNodes();

	glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
}

Renderer2::~Renderer2()
{
	delete sceneShader;
	delete skeletonShader;
	delete soildColorShader;

	delete glassMat;
	delete SoildColorMat;
	delete SkeletonMat;

	delete root;
	delete quad;
	delete camera;

	delete light[0];
	delete light[1];

	glDeleteTextures(1,&stainedGlassTex);
	glDeleteTextures(3, hellKnightTex);
	glDeleteTextures(3, brickTex);
	glDeleteTextures(2, shadowTex);

	glDeleteFramebuffers(2, shadowFrameBuffer);
	CubeRobot::DeleteCube();
	delete skeletonData;
}

void Renderer2::UpdateScene(float msec)
{
	//auto rotate
	cameraRotation += 0.01f*msec;
	Vector3 currentPoint = Matrix4::Rotation(cameraRotation, Vector3(0.0f, 1.0f, 0.0f))*Vector3(0, 500, 800);
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 target = Vector3();
	Matrix4 lookatMatrix = Matrix4::BuildViewMatrix(currentPoint, target);
	camera->SetPosition(currentPoint);
	viewMatrix = lookatMatrix;

	////manually rotate
	//camera->UpdateCamera(msec);
	//viewMatrix = camera->BuildViewMatrix();

	Vector3 todes1 = (des[(index + 1) % 4]- currentLocation);
	Vector3 todes2 = (des[(index + 1) % 4] - des[(index) % 4]);
	if (Vector3::Dot(todes1, todes2) < 0)
	{
		index++;
		currentLocation = des[(index) % 4];
		direction = (des[(index + 1) % 4] - des[(index) % 4]).GetNormal();
	}
	currentLocation = currentLocation + direction*walkSpeed*msec;
	controlMonster->SetTransform(Matrix4::Translation(currentLocation)*Matrix4::Rotation(rotation[(index) % 4], Vector3(0.0f, 1.0f, 0.0f)));

	//viewMatrix = lookatMatrix;
	frameFrustum.FromMatrix(projMatrix*viewMatrix);
	root->Update(msec);
	
	//update material 
	SkeletonMat->SetCameraPos(camera->GetPosition());
	SkeletonMat->SetLightParameter1(light[0]->colour, light[0]->position, light[0]->radius);
	SkeletonMat->SetLightParameter2(light[1]->colour, light[1]->position, light[1]->radius);
	planeMat->SetCameraPos(camera->GetPosition());
	planeMat->SetLightParameter1(light[0]->colour, light[0]->position, light[0]->radius);
	planeMat->SetLightParameter2(light[1]->colour, light[1]->position, light[1]->radius);

}

void Renderer2::DrawNode(SceneNode *n)
{
	if (n->GetMesh())
	{
		modelMatrix = n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale());
		textureMatrix = n->GetTextureMatrix();
		n->GetMaterial()->SetCurrentShader(&currentShader);
		n->Draw(*this);
	}
}
void Renderer2::SetRenderOrder(SceneNode * from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++)
	{
		SetRenderOrder(*i);
	}
}

void Renderer2::SortNodeLists()
{
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CampareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CampareByCameraDistance);
}

void Renderer2::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer2::DrawNodes()
{
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); i++)
	{
		DrawNode(*i);
	}

	glEnable(GL_BLEND);
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); i++)
	{
		DrawNode(*i);
	}
	glDisable(GL_BLEND);

}

void Renderer2::InitQuad()
{
	quad = Mesh::GenerateQuad();
	glassMat->SetTexture(stainedGlassTex);
	for (int i = 0; i < 7; i++)
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(i < 3 ? 200.0f : -200, 100.0f, 150 - 200.0f*(i % 3))));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetMesh(quad);
		s->SetMaterial(glassMat);
		root->AddChild(s);
	}
	for (int i = 0; i < 4; i++)
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(i < 2 ? 400.0f : -400, 100.0f, 100 - 400.0f*(i % 2))) * Matrix4::Rotation(90, Vector3(0.0f, 1.0f, 0.0f)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetMesh(quad);
		s->SetMaterial(glassMat);
		root->AddChild(s);
	}

	planeMat->SetDiffuseTex(brickTex[0]);
	planeMat->SetBumpTex(brickTex[1]);
	planeMat->SetSpecularTex(brickTex[2]);
	planeMat->SetAmbient(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	planeMat->SetCameraPos(camera->GetPosition());
	planeMat->SetLightParameter1(light[0]->colour, light[0]->position, light[0]->radius);
	planeMat->SetLightParameter2(light[1]->colour, light[1]->position, light[1]->radius);

	SceneNode* s = new SceneNode();
	s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	s->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f)) /**Matrix4::Scale(Vector3(25.0f, 25.0f, 25.0f))*/);
	s->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
	s->SetTextureMatrix(Matrix4::Scale(Vector3(5.0f,5.0f,1.0f)));
	s->SetMesh(quad);
	s->SetMaterial(planeMat);
	root->AddChild(s);
}

void Renderer2::InitRobot()
{
	for (int i = 0; i < 3; i++)
	{
		CubeRobot* r = new CubeRobot(SoildColorMat);
		r->SetTransform(Matrix4::Translation(Vector3(0.0f, 0.0f, -300 + 200.0f*(i))));
		root->AddChild(r);
	}
}

void Renderer2::InitMonster()
{
	SkeletonMat->SetDiffuseTex(hellKnightTex[0]);
	SkeletonMat->SetBumpTex(hellKnightTex[1]);
	SkeletonMat->SetSpecularTex(hellKnightTex[2]);
	SkeletonMat->SetAmbient(Vector4(0.0f,0.0f,0.0f,1.0f));
	SkeletonMat->SetCameraPos(camera->GetPosition());
	SkeletonMat->SetLightParameter1(light[0]->colour, light[0]->position, light[0]->radius);
	SkeletonMat->SetLightParameter2(light[1]->colour, light[1]->position, light[1]->radius);

	skeletonData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	skeletonData->AddAnim(MESHDIR"walk7.md5anim");
	skeletonData->AddAnim(MESHDIR"idle2.md5anim");
	skeletonData->AddAnim(MESHDIR"attack2.md5anim");
	string anim[] = { MESHDIR"walk7.md5anim" ,MESHDIR"idle2.md5anim" ,MESHDIR"attack2.md5anim" };

	for (int i = 0; i < 3; i++)
	{
		skeletonNode = new MD5Node(*skeletonData);
		skeletonNode->PlayAnim(anim[i]);
		skeletonNode->SetModelScale(Vector3(1.0f, 1.0f, 1.0f));
		skeletonNode->SetTransform(Matrix4::Translation(Vector3(-300.0f + i * 200.0f, 0.0f, 200.0f + i * -200.0f)));
		skeletonNode->SetMaterial(SkeletonMat);
		skeletonNode->SetBoundingRadius(200.0f);
		skeletonNode->SetIsBakeIntoPose(true);
		root->AddChild(skeletonNode);
	}

	skeletonNode = new MD5Node(*skeletonData);
	skeletonNode->PlayAnim(anim[0]);
	skeletonNode->SetModelScale(Vector3(1.0f, 1.0f, 1.0f));
	skeletonNode->SetTransform(Matrix4::Translation(Vector3(300, 0.0f, 200))*Matrix4::Rotation(90, Vector3(0.0f, 1.0f, 0.0f)));
	skeletonNode->SetMaterial(SkeletonMat);
	skeletonNode->SetBoundingRadius(200.0f);
	skeletonNode->SetIsBakeIntoPose(false);
	root->AddChild(skeletonNode);

	des[0] = Vector3(-200.0f, 0.0f, -300.0f);
	rotation[0] = 90;
	des[1] = Vector3(200.0f, 0.0f, -300.0f);
	rotation[1] = 0;
	des[2] = Vector3(200.0f, 0.0f, 300.0f);
	rotation[2] = -90;
	des[3] = Vector3(-200.0f, 0.0f, 300.0f);
	rotation[3] = -180;
	index = 0;
	walkSpeed = 0.1f;
	currentLocation = des[0];
	direction = (des[1] - des[0]).GetNormal();
	controlMonster = new MD5Node(*skeletonData);
	controlMonster->PlayAnim(MESHDIR"walk7.md5anim");
	controlMonster->SetModelScale(Vector3(1.0f, 1.0f, 1.0f));
	controlMonster->SetTransform(Matrix4::Translation(des[0])*Matrix4::Rotation(rotation[0], Vector3(0.0f, 1.0f, 0.0f)));
	controlMonster->SetMaterial(SkeletonMat);
	controlMonster->SetBoundingRadius(200.0f);
	controlMonster->SetIsBakeIntoPose(true);
	root->AddChild(controlMonster);

}

void Renderer2::DrawShadowMap()
{
	Matrix4 tempMatrix = viewMatrix;

	glViewport(0, 0, shadowSize.x, shadowSize.y);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex[i], 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		if (i == 0) //parallel
		{
			Vector3 dir = (-static_cast<ParallelLight*>(light[0])->position);
			dir.Normalise();
			projMatrix = Matrix4::Orthographic(100, 1100, 1000, -1000, -1000, 1000);
			viewMatrix = Matrix4::BuildViewMatrix((dir * 600.0f), Vector3(0.0f, 0.0f, 0.0f));
		}
		else if (i == 1) // point
		{
			projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 90.0f);
			viewMatrix = Matrix4::BuildViewMatrix(static_cast<PointLight*>(light[1])->position, Vector3(0.0f, 0.0f, 0.0f));
		}
		
		shadowMatrix[i] = biasMatrix * (projMatrix * viewMatrix);
		for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); i++)
		{
			DrawNode(*i);
		}
	}
	SkeletonMat->SetTexMatrix1(shadowTex[0],&shadowMatrix[0]);
	SkeletonMat->SetTexMatrix2(shadowTex[1], &shadowMatrix[1]);
	planeMat->SetTexMatrix1(shadowTex[0], &shadowMatrix[0]);
	planeMat->SetTexMatrix2(shadowTex[1], &shadowMatrix[1]);

	viewMatrix = tempMatrix;
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}