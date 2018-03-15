#include "CubeRobot.h"

Mesh* CubeRobot::cube = nullptr;

CubeRobot::CubeRobot(SoildColorMaterial* mat)
{
	CubeNode* body = new CubeNode(cube, mat, Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	AddChild(body);

	head = new CubeNode(cube, mat, Vector4(0, 1, 0, 1));
	head->SetModelScale(Vector3(5, 5, 5));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->AddChild(head);

	leftArm = new CubeNode(cube, mat, Vector4(0, 0, 1, 1));
	leftArm->SetModelScale(Vector3(3, -18, 3));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->AddChild(leftArm);

	rightArm = new CubeNode(cube, mat, Vector4(0, 0, 1, 1));
	rightArm->SetModelScale(Vector3(3, -18, 3));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->AddChild(rightArm);

	CubeNode* leftLeg = new CubeNode(cube, mat, Vector4(0, 0, 1, 1));
	leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->AddChild(leftLeg);

	CubeNode* rightLeg = new CubeNode(cube, mat, Vector4(0, 0, 1, 1));
	rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	body->AddChild(rightLeg);


}


CubeRobot::~CubeRobot()
{

}

void CubeRobot::Update(float msec)
{
	transform = transform * Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));

	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(0, 1, 0)));

	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));

	rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));

	SceneNode::Update(msec);
}



void CubeRobot::Draw()
{
	if (mesh != nullptr)
	{
		mesh->SetMatrial(material);
		static_cast<SoildColorMaterial*>(material)->SetSoildColor(colour);
		mesh->Draw();
	}
}
