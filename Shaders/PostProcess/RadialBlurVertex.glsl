# version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 colour;

out Vertex{
	vec2 texCoord;
	vec2 uv;
}OUT;

void main()
{
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);

	gl_Position = sign( gl_Position );
	//transform [-1,1] to [0,1]
	OUT.uv = (gl_Position.xy + vec2(1.0)) / vec2(2.0);

	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

}
