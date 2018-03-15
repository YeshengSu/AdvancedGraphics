#pragma once
#include "CubeNode.h"

#include "OBJMesh.h"

class CubeRobot : public SceneNode
{
public:
	CubeRobot(SoildColorMaterial* mat);
	virtual ~CubeRobot();
	virtual void Update(float msec);
	virtual void Draw()override;
	static void CreateCube() {
		OBJMesh*m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		cube = m;
	}
	static void DeleteCube() { delete cube; }

protected :
	static Mesh* cube;
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;

};

