# version 150 core

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex {
	vec2 texCoord;
	vec4 colour;
}IN;

out vec4 FragColor;

const float weights[5] = float[](0.20, 0.23, 0.26, 0.23, 0.20);

void main(){
	vec2 values[5];
	if (isVertical == 1)
	{
		values = vec2[](vec2(0.0, -pixelSize.y * 4),
			vec2(0.0, -pixelSize.y * 2),
			vec2(0.0, pixelSize.y * 1),
			vec2(0.0, pixelSize.y * 2),
			vec2(0.0, pixelSize.y * 4));
	}
	else
	{
		values = vec2[](vec2(-pixelSize.x * 4, 0.0),
			vec2(-pixelSize.x * 2, 0.0),
			vec2(pixelSize.x * 1, 0.0),
			vec2(pixelSize.x * 2, 0.0),
			vec2(pixelSize.x * 4, 0.0));
	}
	
	FragColor = vec4(0);
	for (int i = 0; i < 5; i++)
	{
		vec4 temp = texture(diffuseTex, IN.texCoord.xy + values[i]);
		FragColor += temp * weights[i];
	}
}