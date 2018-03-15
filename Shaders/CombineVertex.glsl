# version 150 core
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;

out vec2 texCoordinate;

void main()
{
	gl_Position = projMatrix * vec4(position, 1.0);

	texCoordinate = texCoord;
	
}