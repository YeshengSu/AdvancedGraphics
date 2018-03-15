# version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
	float height;
}OUT;

void main()
{
	vec4 worldPos = modelMatrix*vec4(position, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	OUT.colour = colour;

	OUT.texCoord = (textureMatrix* vec4(texCoord, 0.0, 1.0)).xy;

	OUT.normal = normalize(normalMatrix * normalize(normal));

	OUT.tangent = normalize(normalMatrix * normalize(tangent));

	OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));
	
	OUT.worldPos = worldPos.xyz;

	OUT.shadowProj = (shadowMatrix * vec4(position + (normal * 25), 1));

	OUT.height = position.y;

	gl_Position = projMatrix * viewMatrix * worldPos;
}