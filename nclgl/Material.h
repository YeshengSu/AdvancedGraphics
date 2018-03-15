#pragma once

#include "OGLRenderer.h"

class Material
{
public:
	Material(Shader* shader);
	virtual ~Material() {}

	void SetCurrentShader(Shader** currentShader);

	void SetMatrix(Matrix4* projMatrix, Matrix4* modelMatrix, Matrix4* viewMatrix, Matrix4* textureMatrix);

	void SetShadowMatrix(Matrix4* shadowMatrix);

	virtual void ApplyMaterial();

	Shader* GetShader() { return shader; }

protected :

	void UpdateMatrix();

	Matrix4* shadowMatrix;

	Matrix4* projMatrix;		//Projection matrix
	Matrix4* modelMatrix;	//Model matrix. NOT MODELVIEW
	Matrix4* viewMatrix;		//View matrix
	Matrix4* textureMatrix;	//Texture matrix

	Shader* shader;
};


class PostProcessMaterial : public Material
{
public :
	PostProcessMaterial(Shader* shader);
	virtual ~PostProcessMaterial() { }

	virtual void ApplyMaterial()override;

	void SetTexture(GLuint texture);

	void SetBrightness(float brightness);

protected :

	float brightness;

	GLuint texture;

};
typedef PostProcessMaterial SceneMaterial;
typedef PostProcessMaterial SunMaterial;
typedef PostProcessMaterial Sprite2D;

class TerrainMaterial : public Material
{
public :
	TerrainMaterial(Shader* shader);
	virtual ~TerrainMaterial(){}

	void SetTerrainHeight(float terrainHeight);

	void SetRockTexture(GLuint rockDiffuseTex, GLuint rockDumpTex, GLuint rockSpecTex);

	void SetGrassTexture(GLuint grassDiffuseTex, GLuint grassDumpTex, GLuint grassSpecTex);

	void SetSnowTexture(GLuint snowDiffuseTex, GLuint snowDumpTex, GLuint snowSpecTex);

	void SetCameraPos(Vector3 cameraPos);

	void SetAmbient(Vector4 ambient);

	void SetLightParameter(Vector4 colour, Vector3 lightPosOrDirection, float lightRadius);
	
	void SetClipPlane(Vector4 clipPlane);

	void SetShadowTex(int shadowTex);

	virtual void ApplyMaterial()override;
protected :

	Vector4 clipPlane;

	GLuint shadowTex;

	GLuint rockDiffuseTex;
	GLuint grassDiffuseTex;
	GLuint snowDiffuseTex;

	GLuint rockDumpTex;
	GLuint grassDumpTex;
	GLuint snowDumpTex;

	GLuint rockSpecTex;
	GLuint grassSpecTex;
	GLuint snowSpecTex;

	float terrainHeight;

	Vector3 cameraPos;

	Vector4 ambient;

	Vector4 lightColor;
	Vector4 lightPosAndRadius;
	
};

class WaterMaterial : public Material
{
public :
	WaterMaterial(Shader* shader);
	~WaterMaterial(){}

	virtual void ApplyMaterial()override;

	void SetDudvMap(int dudvMap);

	void SetDumpMap(int dumpMap);

	void SetReflectionTexture(int reflectionTex);

	void SetRefractionTexture(int refractionTex);

	void SetDepthMapAndPlane(int depthMap,float farPlane, float nearPlane);

	void SetMoveFactor(float moveFactor);

	void SetCameraPos(Vector3 cameraPos);

	void SetAmbient(Vector4 ambient);

	void SetLightParameter(Vector4 colour, Vector3 lightPosOrDirection, float lightRadius);
protected :
	
	GLuint reflectionTex;
	GLuint refractionTex;

	GLuint depthMap;
	float farPlane;
	float nearPlane;

	GLuint dumpMap;
	GLuint dudvMap;

	float moveFactor;

	Vector3 cameraPos;

	Vector4 ambient;

	Vector4 lightColor;
	Vector4 lightPosAndRadius;
};

class SkyboxMaterial : public Material
{
public :
	SkyboxMaterial(Shader* shader);
	~SkyboxMaterial(){}

	virtual void ApplyMaterial()override;

	void SetCubeTex(int cubeTex);

protected :
	GLuint cubeTex;
};

class SoildColorMaterial : public Material
{
public :
	SoildColorMaterial(Shader* shader);
	~SoildColorMaterial(){}

	virtual void ApplyMaterial()override;

	void SetSoildColor(Vector4 soildColor);

protected :
	Vector4 soildColor;
};


class DTerrainMaterial : public TerrainMaterial
{
public :
	DTerrainMaterial(Shader* shader);

	~DTerrainMaterial(){}

	virtual void ApplyMaterial()override;
};

class DLightMaterial : public Material
{
public :
	DLightMaterial(Shader* shader);

	~DLightMaterial() {}

	virtual void ApplyMaterial()override;

	void SetLightParameter(Vector4 colour, Vector3 lightPosOrDirection, float lightRadius);

	void SetPixelSize(Vector2 pixelSize);

	void SetCameraPos(Vector3 cameraPos);

	void SetDepthTex(GLuint depthTex);

	void SetNormalTex(GLuint normTex);

protected :

	GLuint depthTex;
	GLuint normTex;

	Vector2 pixelSize;
	Vector3 cameraPos;
	Vector4 lightColor;
	Vector4 lightPosAndRadius;
};

class DCombineMaterial : public Material
{
public :
	DCombineMaterial(Shader* shader);

	~DCombineMaterial(){}

	virtual void ApplyMaterial()override;

	void SetDiffuseTex(int diffuseTex);
	void SetEmissiveTex(int emissiveTex);
	void SetSpecularTex(int SpecularTex);
	void SetPostSpecularTex(int postSpeculartex);
protected:

	GLuint diffuseTex;
	GLuint emissiveTex;
	GLuint specularTex;
	GLuint postSpeculartex;

};

class TextureMaterial : public Material
{
public :
	TextureMaterial(Shader* shader);
	~TextureMaterial(){}

	virtual void ApplyMaterial()override;

	void SetDiffuseTex(int diffuseTex);
	void SetBumpTex(int bumpTex);
	void SetSpecularTex(int specTex);
	void SetShadowTex(int shadowTex);

	void SetAmbient(Vector4 ambient);
	void SetCameraPos(Vector3 cameraPos);
	void SetLightParameter1(Vector4 lightColour, Vector3 lightPosOrDirection, float lightRadius);
	void SetLightParameter2(Vector4 lightColour, Vector3 lightPosOrDirection, float lightRadius);

	void SetTexMatrix1(GLuint shadowTex, Matrix4* matrix);
	void SetTexMatrix2(GLuint shadowTex, Matrix4* matrix);

protected :
	GLuint diffuseTex;
	GLuint bumpTex;
	GLuint specTex;

	GLuint shadowTex[2];
	Matrix4* newShadowMatrix[2];

	Vector4 ambient;
	Vector3 cameraPos;
	Vector4 lightColour[2];
	Vector4 lightPosAndRadius[2];
};

class ParticleMaterial : public PostProcessMaterial
{
public :
	ParticleMaterial(Shader* shader);
	virtual ~ParticleMaterial();

	virtual void ApplyMaterial()override;

	void SetBufferTex(GLuint texture);

protected :
	GLuint bufferTex;
};

class StandardMaterial : public Material
{
public:
	StandardMaterial(Shader* shader);
	~StandardMaterial() {}

	virtual void ApplyMaterial()override;

	void SetDiffuseTex(int diffuseTex);
	void SetBumpTex(int bumpTex);
	void SetSpecularTex(int specTex);
	void SetShadowTex(int shadowTex,float shadowBias);

	void SetAmbient(Vector4 ambient);
	void SetCameraPos(Vector3 cameraPos);
	void SetLightParameter(Vector4 lightColour, Vector3 lightPosOrDirection, float lightRadius);

	void SetCubeTex(int cubeTex, float smoothness);

protected:
	GLuint cubeTex;
	float smoothness;

	GLuint diffuseTex;
	GLuint bumpTex;
	GLuint specTex;

	GLuint shadowTex;
	float shadowBias;

	Vector4 ambient;
	Vector3 cameraPos;
	Vector4 lightColour;
	Vector4 lightPosAndRadius;
};