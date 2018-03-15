#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include "Material.h"

class SceneNode
{
public:
	SceneNode(Mesh *m = nullptr, Material* mat = nullptr, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	virtual ~SceneNode();

	void SetTransform(const Matrix4 &matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform()const { return worldTransform; }

	Vector4 GetColour()const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale()const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh()const { return mesh; }
	void SetMesh(Mesh* m);

	Material* GetMaterial() { return material; }
	void SetMaterial(Material* mat) { material = mat; }

	void AddChild(SceneNode* s);

	virtual void Update(float msec);
	virtual void Draw();

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

	float GetBoundingRadius()const { return boundingRadius * max(max(abs(modelScale.x), abs(modelScale.y)), abs(modelScale.z)); }
	void  SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance()const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTextureMatrix(const Matrix4 &textureMat) { this->textureMat = textureMat; }
	const Matrix4& GetTextureMatrix() const { return textureMat; }

	static bool CampareByCameraDistance(SceneNode* a, SceneNode* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

protected :
	SceneNode* parent;
	Mesh*	   mesh;
	Matrix4    worldTransform;
	Matrix4    transform;
	Matrix4    textureMat;
	Vector3    modelScale;
	Vector4    colour;
	Material*   material;

	float distanceFromCamera;
	float boundingRadius;

	std::vector<SceneNode*> children; 

};

