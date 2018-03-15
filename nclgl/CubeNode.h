#pragma once
#pragma once
#include "SceneNode.h"
#include "OBJMesh.h"

class CubeNode : public SceneNode
{
public:
	CubeNode(Mesh *m = nullptr, Material* mat = nullptr, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	virtual ~CubeNode();
	virtual void Draw()override;
};

