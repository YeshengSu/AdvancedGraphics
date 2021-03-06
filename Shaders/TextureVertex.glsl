# version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix1;
uniform mat4 shadowMatrix2;

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj[2];
}OUT;

void main()
{
	vec4 worldPos = modelMatrix*vec4(position, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	OUT.texCoord = (textureMatrix* vec4(texCoord, 0.0, 1.0)).xy;

	OUT.normal = normalize(normalMatrix * normalize(normal));

	OUT.tangent = normalize(normalMatrix * normalize(tangent));

	OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));
	
	OUT.worldPos = worldPos.xyz;

	vec4 tempNor = normalize(modelMatrix*vec4(normal, 1.0));
	OUT.shadowProj[0] = (shadowMatrix1 * vec4(worldPos.xyz + (tempNor.xyz * 1.5), 1));
	OUT.shadowProj[1] = (shadowMatrix2  * vec4(worldPos.xyz + (tempNor.xyz * 1.5), 1));

	gl_Position = projMatrix * viewMatrix * worldPos;
}