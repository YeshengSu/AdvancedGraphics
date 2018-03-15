# version 150 core

uniform sampler2D diffuseTex;
uniform float brightness;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main()
{
	FragColor = texture(diffuseTex,IN.texCoord) * brightness;
	//FragColor.r /= FragColor.w;
	//FragColor.g /= FragColor.w;
	//FragColor.b /= FragColor.w;
	//FragColor.w = 1.0f;
}


