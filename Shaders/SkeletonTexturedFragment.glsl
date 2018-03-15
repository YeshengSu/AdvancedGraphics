# version 330 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;//
	vec3 worldPos;
	vec4 shadowProj;//
}IN;

out vec4 FragColor;

void main(){
	FragColor =texture(diffuseTex,IN.texCoord);
}