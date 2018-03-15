#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec4 colour;
} IN;

out vec4 FragColor;

void main(void)	{
	FragColor = IN.colour;
}