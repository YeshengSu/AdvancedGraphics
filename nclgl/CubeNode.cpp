#include "CubeNode.h"

CubeNode::CubeNode(Mesh * m, Material * mat, Vector4 color)
	:SceneNode(m,mat,color)
{
}

CubeNode::~CubeNode()
{
}

void CubeNode::Draw()
{
	if (mesh != nullptr)
	{
		mesh->SetMatrial(material);
		static_cast<SoildColorMaterial*>(material)->SetSoildColor(colour);
		mesh->Draw();
	}
}
