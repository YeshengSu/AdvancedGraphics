# version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D postspecularTex;

in vec2 texCoordinate;
out vec4 FragColor;

void main(void)
{
	vec3 diffuse = texture(diffuseTex, texCoordinate).xyz;
	vec3 light = texture(emissiveTex, texCoordinate).xyz;
	vec3 specular = texture(specularTex, texCoordinate).xyz;
	vec3 postSpecular = texture(postspecularTex, texCoordinate).xyz;

	FragColor.xyz = diffuse * 0.2;
	FragColor.xyz += diffuse * light;
	FragColor.xyz += specular;
	FragColor.xyz += postSpecular;
	FragColor.a = 1.0f;

}
