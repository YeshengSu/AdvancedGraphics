#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

/*
We need two uniforms per object, containing the packed information
we got from the Texture Atlas and its node.
*/
uniform vec3 atlasInfo;
uniform vec4 textureInfo;

in  vec3 position;
in  vec2 texCoord;
in  vec4 colour;

out Vertex	{
	vec2 texCoord;
	vec4 colour;
} OUT;

/*
With the node and atlas info, we can generate the correct texture coordinates
for a given texture using exactly the same formula as we do in the AtlasTreeNode
VertexCoordsToAtlasCoords function!
*/
vec2 VertexCoordsToAtlasCoords() {
	//Node info:  Vector4((float)position.x,(float)position.y,(float)size.x,(float)size.y);
	//Atlas Info: Vector3((float)size.x,(float)size.y,(float)border);
	vec2 coords;

	coords.x = textureInfo.x / atlasInfo.x;
	coords.y = textureInfo.y / atlasInfo.y;

	coords.x += ((textureInfo.z - atlasInfo.z) / atlasInfo.x) * texCoord.x;
	coords.y += ((textureInfo.w - atlasInfo.z) / atlasInfo.y) * texCoord.y;

	return coords;
}

/*
With the function defined, the rest of the vertex shader is as you're used to,
only now we get the texCoord from a call to our new function. Easy!
*/
void main(void)	{
	gl_Position		= (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
	OUT.texCoord	= VertexCoordsToAtlasCoords();
	OUT.colour		= colour;
}