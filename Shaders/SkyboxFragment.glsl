# version 150 core

uniform samplerCube cubeTex;
uniform vec3 cameraPos;

in Vertex {
	vec3 normal;
} IN;

out vec4 FragColor;

void main()
{
	FragColor = texture(cubeTex,normalize(IN.normal));
}


