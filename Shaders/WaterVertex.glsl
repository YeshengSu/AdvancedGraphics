#version 150 core 

uniform mat4 modelMatrix; 
uniform mat4 viewMatrix; 
uniform mat4 projMatrix; 
uniform mat4 textureMatrix;

in vec3 position; 
in vec4 colour; 
in vec2 texCoord;

out Vertex { 
	vec4 clipSpace; // use to find out uv according to screen xy
	vec4 colour;
	vec3 worldPos;
	vec2 texCoord;
} OUT;

const float tiling = 20.0f;

 void main(void) { 
	 vec4 worldPos = modelMatrix * vec4(position, 1.0);

	 gl_Position = projMatrix * viewMatrix * worldPos;

	 OUT.clipSpace = gl_Position;
	 OUT.colour = colour; 
	 OUT.worldPos = worldPos.xyz;
	 OUT.texCoord = (texCoord * 0.5 + 0.5) * tiling;
}