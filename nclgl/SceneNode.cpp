#include "SceneNode.h"


SceneNode::SceneNode(Mesh *m, Material* mat, Vector4 color)
{
	this->mesh = m;
	this->colour = color;
	this->material = mat;
	parent = nullptr;
	modelScale = Vector3(1.0f, 1.0f, 1.0f);
	distanceFromCamera = 0.0f;

	if (m != nullptr)
		boundingRadius = m->GetBoundingRadius();

	return;
}

SceneNode::~SceneNode()
{
	for (unsigned int i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode * s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Update(float msec)
{
	if (parent)
		worldTransform = parent->worldTransform*transform;
	else
		worldTransform = transform;

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); i++)
	{
		(*i)->Update(msec);
	}
}

void SceneNode::Draw()
{
	if (mesh != nullptr)
	{
		mesh->SetMatrial(material);
		mesh->Draw();
	}
}

void SceneNode::SetMesh(Mesh* m)
{
	mesh = m;
	boundingRadius = mesh->GetBoundingRadius();
}