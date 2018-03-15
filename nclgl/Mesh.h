#pragma once
#include "OGLRenderer.h"

class Material;

enum MeshBuffer 
{
	VERTEX_BUFFER,
	COLOUR_BUFFER,
	TEXTURE_BUFFER,
	NORMAL_BUFFER,
	TANGENT_BUFFER,
	INDEX_BUFFER,
	MAX_BUFFER
};

class Mesh
{
public :
	Mesh();
	~Mesh();

	virtual void Draw();

	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	void SetMatrial(Material* material);
	Material* GetMatrial();
	float GetBoundingRadius();

protected :
	void BufferData();

	void GenerateNormals();
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b, const Vector3& c,
							const Vector2& ta,const Vector2& tb,const Vector2& tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint numIndices;
	GLuint type;

	Material* material;

	Vector3* normals;
	Vector3* vertices;
	Vector4* colours;
	Vector2* textureCoords;
	Vector3* tangents;

	unsigned int* indices;

	float boundingRadius;
};

