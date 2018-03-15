#include "Material.h"

Material::Material(Shader * shader)
{
	this->shader = shader;
	shadowMatrix = nullptr;
}

void Material::SetCurrentShader(Shader ** currentShader)
{
	(*currentShader) = shader;
}

void Material::SetMatrix(Matrix4 * projMatrix, Matrix4 * modelMatrix, Matrix4 * viewMatrix, Matrix4 * textureMatrix)
{
	this->projMatrix = projMatrix;
	this->modelMatrix = modelMatrix;
	this->viewMatrix = viewMatrix;
	this->textureMatrix = textureMatrix;
}

void Material::SetShadowMatrix(Matrix4 * shadowMatrix)
{
	this->shadowMatrix = shadowMatrix;
}

void Material::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();
}

void Material::UpdateMatrix()
{
	if (modelMatrix != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, (float*)modelMatrix);
	if (viewMatrix != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMatrix"), 1, false, (float*)viewMatrix);
	if (projMatrix != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "projMatrix"), 1, false, (float*)projMatrix);
	if (textureMatrix != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "textureMatrix"), 1, false, (float*)textureMatrix);
	if (shadowMatrix != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "shadowMatrix"), 1, false, (float*)shadowMatrix);
}



PostProcessMaterial::PostProcessMaterial(Shader * shader)
	:Material(shader)
{
	brightness = 1.0f;
}

void PostProcessMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "brightness"), brightness);

}

void PostProcessMaterial::SetTexture(GLuint texture)
{
	this->texture = texture;
}

void PostProcessMaterial::SetBrightness(float brightness)
{
	this->brightness = brightness;
}


TerrainMaterial::TerrainMaterial(Shader * shader)
	:Material(shader)
{
}

void TerrainMaterial::SetTerrainHeight(float terrainHeight)
{
	this->terrainHeight = terrainHeight;
}

void TerrainMaterial::SetRockTexture(GLuint rockDiffuseTex, GLuint rockDumpTex, GLuint rockSpecTex)
{
	this->rockDiffuseTex = rockDiffuseTex;
	this->rockDumpTex = rockDumpTex;
	this->rockSpecTex = rockSpecTex;
}

void TerrainMaterial::SetGrassTexture(GLuint grassDiffuseTex, GLuint grassDumpTex, GLuint grassSpecTex)
{
	this->grassDiffuseTex = grassDiffuseTex;
	this->grassDumpTex = grassDumpTex;
	this->grassSpecTex = grassSpecTex;
}

void TerrainMaterial::SetSnowTexture(GLuint snowDiffuseTex, GLuint snowDumpTex, GLuint snowSpecTex)
{
	this->snowDiffuseTex = snowDiffuseTex;
	this->snowDumpTex = snowDumpTex;
	this->snowSpecTex = snowSpecTex;
}

void TerrainMaterial::SetCameraPos(Vector3 cameraPos)
{
	this->cameraPos = cameraPos;
}

void TerrainMaterial::SetAmbient(Vector4 ambient)
{
	this->ambient = ambient;
}

void TerrainMaterial::SetLightParameter(Vector4 colour, Vector3 lightPosOrDirection, float lightRadius)
{
	this->lightColor = colour;
	this->lightPosAndRadius = Vector4(lightPosOrDirection.x, lightPosOrDirection.y, lightPosOrDirection.z, lightRadius);
}

void TerrainMaterial::SetClipPlane(Vector4 clipPlane)
{
	this->clipPlane = clipPlane;
}

void TerrainMaterial::SetShadowTex(int shadowTex)
{
	this->shadowTex = shadowTex;
}

void TerrainMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "clipPlane"), 1, (float*)&clipPlane);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex"), 0);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassDiffuseTex"),	1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassBumpTex"),		2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassSpecTex"),		3);

	glUniform1i(glGetUniformLocation(shader->GetProgram(),"rockDiffuseTex"),	4);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockBumpTex"),		5);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockSpecTex"),		6);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "snowDiffuseTex"),	7);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "snowBumpTex"),		8);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "snowSpecTex"),		9);

	glUniform1f(glGetUniformLocation(shader->GetProgram(), "terrainHeight"), terrainHeight);
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&cameraPos);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "ambient"), 1, (float*)&ambient);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightColour"), 1, (float*)&lightColor);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightPosAndRadius"), 1, (float*)&lightPosAndRadius);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassDiffuseTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassDumpTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassSpecTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, rockDiffuseTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rockDumpTex);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, rockSpecTex);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, snowDiffuseTex);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, snowDumpTex);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, snowSpecTex);
}

WaterMaterial::WaterMaterial(Shader * shader)
	:Material(shader)
{
}
void WaterMaterial::SetDudvMap(int dudvMap)
{
	this->dudvMap = dudvMap;
}

void WaterMaterial::SetDumpMap(int dumpMap)
{
	this->dumpMap = dumpMap;
}

void WaterMaterial::SetReflectionTexture(int reflectionTex)
{
	this->reflectionTex = reflectionTex;
}

void WaterMaterial::SetRefractionTexture(int refractionTex)
{
	this->refractionTex = refractionTex;
}

void WaterMaterial::SetDepthMapAndPlane(int depthMap, float farPlane, float nearPlane)
{
	this->depthMap = depthMap;
	this->farPlane = farPlane;
	this->nearPlane = nearPlane;
}

void WaterMaterial::SetMoveFactor(float moveFactor)
{
	this->moveFactor = moveFactor;
}

void WaterMaterial::SetCameraPos(Vector3 cameraPos)
{
	this->cameraPos = cameraPos;
}

void WaterMaterial::SetAmbient(Vector4 ambient)
{
	this->ambient = ambient;
}

void WaterMaterial::SetLightParameter(Vector4 colour, Vector3 lightPosOrDirection, float lightRadius)
{
	this->lightColor = colour;
	this->lightPosAndRadius = Vector4(lightPosOrDirection.x, lightPosOrDirection.y, lightPosOrDirection.z, lightRadius);
}

void WaterMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "moveFactor"), moveFactor);
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&cameraPos);

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightColour"), 1, (float*)&lightColor);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightPosAndRadius"), 1, (float*)&lightPosAndRadius);

	glUniform1f(glGetUniformLocation(shader->GetProgram(), "farPlane"), farPlane);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "nearPlane"), nearPlane);

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "ambient"), 1, (float*)&ambient);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "reflectionTexture"),	0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "refractionTexture"),	1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "dudvMap"),				2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "dumpMap"),				3);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "depthMap"),				4);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, dumpMap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

SkyboxMaterial::SkyboxMaterial(Shader * shader)
	:Material(shader)
{
}

void SkyboxMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
}

void SkyboxMaterial::SetCubeTex(int cubeTex)
{
	this->cubeTex = cubeTex;
}

SoildColorMaterial::SoildColorMaterial(Shader * shader)
	:Material(shader)
{
}

void SoildColorMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "soildColor"), 1, (float*)&soildColor);
}

void SoildColorMaterial::SetSoildColor(Vector4 soildColor)
{
	this->soildColor = soildColor;
}

DTerrainMaterial::DTerrainMaterial(Shader * shader)
	:TerrainMaterial(shader)
{

}

void DTerrainMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex"), 0);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassDiffuseTex"), 1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassBumpTex"), 2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassSpecTex"), 3);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockDiffuseTex"), 4);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockBumpTex"), 5);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "rockSpecTex"), 6);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "snowDiffuseTex"), 7);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "snowBumpTex"), 8);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "snowSpecTex"), 9);

	glUniform1f(glGetUniformLocation(shader->GetProgram(), "terrainHeight"), terrainHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassDiffuseTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassDumpTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassSpecTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, rockDiffuseTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rockDumpTex);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, rockSpecTex);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, snowDiffuseTex);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, snowDumpTex);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, snowSpecTex);
}

DLightMaterial::DLightMaterial(Shader * shader)
	:Material(shader)
{
}

void DLightMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();

	glUniform2fv(glGetUniformLocation(shader->GetProgram(), "pixelSize"), 1, (float*)&pixelSize);
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&cameraPos);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightPosRaidus"), 1, (float*)&lightPosAndRadius);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightColour"), 1, (float*)&lightColor);


	glUniform1i(glGetUniformLocation(shader->GetProgram(), "depthTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "normTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normTex);
}

void DLightMaterial::SetLightParameter(Vector4 colour, Vector3 lightPosOrDirection, float lightRadius)
{
	this->lightColor = colour;
	this->lightPosAndRadius = Vector4(lightPosOrDirection.x, lightPosOrDirection.y, lightPosOrDirection.z, lightRadius);

}

void DLightMaterial::SetPixelSize(Vector2 pixelSize)
{
	this->pixelSize = pixelSize;
}

void DLightMaterial::SetCameraPos(Vector3 cameraPos)
{
	this->cameraPos = cameraPos;
}

void DLightMaterial::SetDepthTex(GLuint depthTex)
{
	this->depthTex = depthTex;
}

void DLightMaterial::SetNormalTex(GLuint normTex)
{
	this->normTex = normTex;
}


DCombineMaterial::DCombineMaterial(Shader * shader)
	:Material(shader)
{
	
}

void DCombineMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	if (projMatrix != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "projMatrix"), 1, false, (float*)projMatrix);


	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "emissiveTex"), 1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "specularTex"), 2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "postspecularTex"), 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, emissiveTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, specularTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, postSpeculartex);

}

void DCombineMaterial::SetDiffuseTex(int diffuseTex)
{
	this->diffuseTex = diffuseTex;
}

void DCombineMaterial::SetEmissiveTex(int emissiveTex)
{
	this->emissiveTex = emissiveTex;
}

void DCombineMaterial::SetSpecularTex(int SpecularTex)
{
	this->specularTex = SpecularTex;
}

void DCombineMaterial::SetPostSpecularTex(int postSpeculartex)
{
	this->postSpeculartex = postSpeculartex;
}


TextureMaterial::TextureMaterial(Shader * shader)
	:Material(shader)
{
	newShadowMatrix[0] = nullptr;
	newShadowMatrix[1] = nullptr;
}

void TextureMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&cameraPos);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightColour"), 2, (float*)&lightColour);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightPosAndRadius"), 2, (float*)&lightPosAndRadius);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "ambient"), 1, (float*)&ambient);

	if (newShadowMatrix[0] != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "shadowMatrix1"), 1, false, (float*)newShadowMatrix[0]);

	if (newShadowMatrix[1] != nullptr)
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "shadowMatrix2"), 1, false, (float*)newShadowMatrix[1]);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex0"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex1"), 1);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 3);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "specTex"), 4);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowTex[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bumpTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, specTex);

}

void TextureMaterial::SetDiffuseTex(int diffuseTex)
{
	this->diffuseTex = diffuseTex;
}

void TextureMaterial::SetBumpTex(int bumpTex)
{
	this->bumpTex = bumpTex;
}

void TextureMaterial::SetSpecularTex(int specTex)
{
	this->specTex = specTex;
}
void TextureMaterial::SetShadowTex(int shadowTex)
{
}
void TextureMaterial::SetAmbient(Vector4 ambient)
{
	this->ambient = ambient;
}

void TextureMaterial::SetCameraPos(Vector3 cameraPos)
{
	this->cameraPos = cameraPos;
}

void TextureMaterial::SetLightParameter1(Vector4 lightColour, Vector3 lightPosOrDirection, float lightRadius)
{
	this->lightColour[0] = lightColour;
	this->lightPosAndRadius[0] = Vector4(lightPosOrDirection.x, lightPosOrDirection.y, lightPosOrDirection.z, lightRadius);

}
void TextureMaterial::SetLightParameter2(Vector4 lightColour, Vector3 lightPosOrDirection, float lightRadius)
{
	this->lightColour[1] = lightColour;
	this->lightPosAndRadius[1] = Vector4(lightPosOrDirection.x, lightPosOrDirection.y, lightPosOrDirection.z, lightRadius);

}

void TextureMaterial::SetTexMatrix1(GLuint shadowTex, Matrix4 * matrix)
{
	this->shadowTex[0] = shadowTex;
	this->newShadowMatrix[0] = matrix;
}

void TextureMaterial::SetTexMatrix2(GLuint shadowTex, Matrix4 * matrix)
{
	this->shadowTex[1] = shadowTex;
	this->newShadowMatrix[1] = matrix;
}

ParticleMaterial::ParticleMaterial(Shader * shader)
	:PostProcessMaterial(shader)
{
}

ParticleMaterial::~ParticleMaterial()
{
}

void ParticleMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "brightness"), brightness);
}

void ParticleMaterial::SetBufferTex(GLuint texture)
{
	bufferTex = texture;
}

StandardMaterial::StandardMaterial(Shader * shader)
	:Material(shader)
{
	smoothness = 0.5f;
	shadowBias = 1.5f;
}

void StandardMaterial::ApplyMaterial()
{
	glUseProgram(shader->GetProgram());
	UpdateMatrix();
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "smoothness"), smoothness);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "shadowBias"), shadowBias);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&cameraPos);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightColour"), 1, (float*)&lightColour);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "lightPosAndRadius"), 1, (float*)&lightPosAndRadius);
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "ambient"), 1, (float*)&ambient);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex"),	0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"),		1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"),	2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"),		3);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "specTex"),		4);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bumpTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, specTex);
}
void StandardMaterial::SetDiffuseTex(int diffuseTex)
{
	this->diffuseTex = diffuseTex;
}
void StandardMaterial::SetBumpTex(int bumpTex)
{
	this->bumpTex = bumpTex;
}
void StandardMaterial::SetSpecularTex(int specTex)
{
	this->specTex = specTex;
}
void StandardMaterial::SetShadowTex(int shadowTex, float shadowBias)
{
	this->shadowTex = shadowTex;
	this->shadowBias = shadowBias;
}
void StandardMaterial::SetAmbient(Vector4 ambient)
{
	this->ambient = ambient;
}
void StandardMaterial::SetCameraPos(Vector3 cameraPos)
{
	this->cameraPos = cameraPos;
}
void StandardMaterial::SetLightParameter(Vector4 lightColour, Vector3 lightPosOrDirection, float lightRadius)
{
	this->lightColour = lightColour;
	this->lightPosAndRadius = Vector4(lightPosOrDirection.x, lightPosOrDirection.y, lightPosOrDirection.z, lightRadius);

}
void StandardMaterial::SetCubeTex(int cubeTex, float smoothness)
{
	this->cubeTex = cubeTex;
	this->smoothness = smoothness;
}