# version 150 core

// Fragment shader
const float sampleDist = 1.0;
const float sampleStrength = 1.5;

uniform sampler2D diffuseTex;
uniform sampler2D noise;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex{
	vec2 texCoord;
	vec2 uv;
}IN;

out vec4 FragColor;

void main(void)
{
	float samples[10];
	samples[0] = -0.08;
	samples[1] = -0.05;
	samples[2] = -0.03;
	samples[3] = -0.02;
	samples[4] = -0.01;
	samples[5] = 0.01;
	samples[6] = 0.02;
	samples[7] = 0.03;
	samples[8] = 0.05;
	samples[9] = 0.08;

	
	//get dir normal and make it range between [-0.5,0.5] where middle value is 0
	//all the origin of normal vector will be [0,0] because middle value is 0 and it is in the centre of screen 
	vec2 dir =  IN.uv - vec2(0.5,0.5);//point to (0.5,0.5)
	float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
	dir = dir / dist;

	//sample 
	vec4 color = texture(diffuseTex, IN.texCoord);
	vec4 sum = color;

	for (int i = 0; i < 10; i++)
		sum += texture(diffuseTex, IN.texCoord + dir * samples[i] * sampleDist);

	sum *= 1.0 / 11.0;
	float t = dist * sampleStrength;
	t = clamp(t, 0.0, 1.0);

	FragColor = mix(color, sum, t);
}