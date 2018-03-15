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

uniform vec4      ambient;
uniform vec3      cameraPos;
uniform vec4      lightColour;
uniform vec4      lightPosAndRadius;

uniform mat4 shadowMatrix;

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

out vec4 FragColor;

vec2 poissonDisk[4] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760)
	);

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

	float shadow = 1.0f;

	if (IN.shadowProj.w > 0.0f)
	{
		shadow = clamp(textureProj(shadowTex, IN.shadowProj), shadowStrenth, 1.0);
	}

	float powFactor = 200.0-180*spec;

	if(lightPosAndRadius.w == 0) // Parallel light
	{    
		vec3 incident = normalize(-lightPosAndRadius.xyz);
		float lambert = max(0.0, dot(incident, normal));
		lambert *= shadow;

		vec3 viewDir      = normalize(cameraPos - IN.worldPos);
		vec3 halfDir      = normalize(incident + viewDir);
				      
		float rFactor     = max(0.0, dot(halfDir, normal));
		float sFactor     = pow(rFactor, powFactor);
				      
		vec3 colour       = (diffuse.rgb * lightColour.rgb);
		colour            += (lightColour.rgb * sFactor) * 0.33;
		FragColor      = vec4(colour * lambert, diffuse.a);
		FragColor.rgb  += (diffuse.rgb * lightColour.rgb) * NoLightFactor;
		
	}
	else // point light
	{
		float dist        = length(lightPosAndRadius.xyz - IN.worldPos);
		float atten       = 1.0 - clamp(dist / lightPosAndRadius.w, 0.0, 1.0);
	
		if (atten == 0.0)
		{
			FragColor.rgb = diffuse.rgb * NoLightFactor;
		}
		else
		{
			vec3 incident = normalize(lightPosAndRadius.xyz - IN.worldPos);
			float lambert = max(0.0, dot(incident, normal));
			lambert *= shadow;

			vec3 viewDir = normalize(cameraPos - IN.worldPos);
			vec3 halfDir = normalize(incident + viewDir);

			float rFactor = max(0.0, dot(halfDir, normal));
			float sFactor = pow(rFactor, powFactor);

			vec3 colour = (diffuse.rgb * lightColour.rgb);
			colour += (lightColour.rgb * sFactor) * 0.33;
			FragColor = vec4(colour * atten * lambert, diffuse.a);
			FragColor.rgb += (diffuse.rgb * lightColour.rgb) * NoLightFactor;
		}
	}

	FragColor += ambient;
}

