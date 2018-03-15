# version 150 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 FragColor;

void main()
{
	FragColor = IN.colour;
	if(useTexture > 0)
	FragColor = texture(diffuseTex,IN.texCoord);
}


