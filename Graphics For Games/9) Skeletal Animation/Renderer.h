#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/Material.h"
class Renderer : public OGLRenderer		{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:

	void Renderer::SetMaterial(SceneNode * from);

	MD5FileData*skeletonData;
	MD5Node*	skeletonNode;
	Camera*		camera;
	SceneMaterial* material;
	GLuint textures;
};

