
#include "Renderer.h"

Renderer::Renderer(Window &parent)
	:OGLRenderer(parent)
{
	CubeRobot::CreateCube();
	camera = new Camera();

	currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");

	if (currentShader->LinkProgram() == 0)
		return;

	projMatrix = Matrix4::Perspective(0.1f, 100000.0f, (float)width / (float)height, 45.0f);

	camera->SetPosition(Vector3(0, 30, 175));

	root = new SceneNode();
	root->AddChild(new CubeRobot());
	root2 = new SceneNode();
	root2->AddChild(new CubeRobot());
	root2->SetTransform(root->GetTransform()*Matrix4::Translation(Vector3(30, 30, 0))*Matrix4::Scale(Vector3(0.5, 0.5, 0.5)));
	glEnable(GL_DEPTH_TEST);
	init = true;
}
Renderer::~Renderer()
{
	delete root;
	delete root2;
	CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);
	root2->Update(msec);
}
void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 1);
	DrawNode(root);
	DrawNode(root2);
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::DrawNode(SceneNode *n)
{
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
			"modelMatrix"), 1, false, (float *)& transform);

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float *)& n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw(*this);

	}

	for (vector < SceneNode * >::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);

	}
}
