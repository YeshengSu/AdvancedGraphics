#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	
	camera			= new Camera(0,-90.0f,Vector3(-180,60,0));

#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader   = new Shader(SHADERDIR"skeletonVertex.glsl", SHADERDIR"SkeletonTexturedFragment.glsl");
#else
	currentShader   = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif

	skeletonData		= new MD5FileData(MESHDIR"hellknight.md5mesh");
	skeletonNode		= new MD5Node(*skeletonData);

	if(!currentShader->LinkProgram()) {
		return;
	}

	skeletonData->AddAnim(MESHDIR"attack2.md5anim");
	skeletonNode->PlayAnim(MESHDIR"attack2.md5anim");

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);

	textures = SOIL_load_OGL_texture(TEXTUREDIR"hellknight.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	
	material = new SceneMaterial(currentShader);
	material->SetTexture(textures);
	skeletonNode->SetMaterial(material);
	material->SetMatrix(&projMatrix,&modelMatrix,&viewMatrix,&textureMatrix);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	init = true;
}

Renderer::~Renderer(void)	{
	delete camera;

	delete skeletonData;
	delete skeletonNode;

	delete material;
}

 void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec); 
	viewMatrix		= camera->BuildViewMatrix();

	skeletonNode->Update(msec);
}

void Renderer::RenderScene()	{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//glUseProgram(currentShader->GetProgram());
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();

	for(int y = 0; y < 10; ++y) {
		for(int x = 0; x < 10; ++x) {
			modelMatrix = Matrix4::Translation(Vector3(x * 100, 0, y * 100));
			material->SetCurrentShader(&currentShader);
			skeletonNode->Draw(*this);
		}
	}

	glUseProgram(0);
	SwapBuffers();
}

void Renderer::SetMaterial(SceneNode * from)
{
	Mesh* mesh = from->GetMesh();
	if (mesh != nullptr)
		mesh->SetMatrial(material);

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++)
		SetMaterial(*i);

}