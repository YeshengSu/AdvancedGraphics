#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor[2];

void main(void)	{
	FragColor[0] = IN.colour* texture(diffuseTex, IN.texCoord);
	FragColor[1] = vec4(0);
}
