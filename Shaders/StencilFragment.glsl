# version 150 core

uniform sampler2D diffuseTex;
in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;
void main()
{
	vec4 value = texture(diffuseTex, IN.texCoord).rgba;
	if(value.a == 0.0f)
		discard;
	gl_FragColor = (1-IN.colour)*(value);
}


