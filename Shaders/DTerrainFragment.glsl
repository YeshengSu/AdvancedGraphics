# version 150 core

uniform sampler2DShadow shadowTex;
//uniform sampler2D shadowTex;
uniform sampler2D grassDiffuseTex;
uniform sampler2D grassBumpTex;
uniform sampler2D grassSpecTex;

uniform sampler2D rockDiffuseTex;
uniform sampler2D rockBumpTex;
uniform sampler2D rockSpecTex;

uniform sampler2D snowDiffuseTex;
uniform sampler2D snowBumpTex;
uniform sampler2D snowSpecTex;

uniform float     terrainHeight;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
	float height;
}IN;

out vec4 FragColor[3];

const float NoLightFactor = 0.1;
const float bias = 0.5f;
const float shadowStrenth = 0.5;

void main(){

	vec4 diffuse = vec4(1);
	vec3 normal = vec3(1);
	vec4 normalRGBA = vec4(1);
	float spec = 0;
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);//??

	float ratio = (IN.height / terrainHeight);
	if (ratio < 0.4)
	{
		diffuse = mix(texture(grassDiffuseTex, IN.texCoord), texture(rockDiffuseTex, IN.texCoord), ratio / 0.4);

		normalRGBA = mix(texture(grassBumpTex, IN.texCoord), texture(rockBumpTex, IN.texCoord), ratio / 0.4);
		normal = normalize(TBN * (normalRGBA.rgb * 2 - 1));

		spec = texture(grassSpecTex, IN.texCoord).r;
	}
	else if (ratio < 0.5)
	{
		diffuse = texture(rockDiffuseTex, IN.texCoord);

		normalRGBA = texture(rockBumpTex, IN.texCoord);
		normal = normalize(TBN * (normalRGBA.rgb * 2 - 1));

		spec = texture(rockSpecTex, IN.texCoord).r;
	}
	else
	{
		diffuse = mix(texture(rockDiffuseTex, IN.texCoord), texture(snowDiffuseTex, IN.texCoord) + vec4(0.2), (ratio - 0.5) / 0.5);

		normalRGBA = mix(texture(rockBumpTex, IN.texCoord), texture(snowBumpTex, IN.texCoord), (ratio - 0.5) / 0.5);
		normal = normalize(TBN * (normalRGBA.rgb * 2 - 1));

		spec = texture(snowSpecTex, IN.texCoord).r;
	}

	FragColor[0] = diffuse;
	FragColor[1] = vec4(normal*0.5+0.5,spec);
}

