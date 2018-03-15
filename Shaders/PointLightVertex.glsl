# version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
out mat4 inverseProjView;

void main()
{
	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	gl_Position = projMatrix * viewMatrix * worldPos;

	inverseProjView = inverse(projMatrix * viewMatrix);
	
}