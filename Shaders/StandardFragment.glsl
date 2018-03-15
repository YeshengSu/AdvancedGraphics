# version 150 core

uniform sampler2DShadow shadowTex;

uniform sampler2D		diffuseTex;
uniform sampler2D		bumpTex;
uniform sampler2D		specTex;
uniform samplerCube		cubeTex;

uniform vec4      ambient;
uniform vec3      cameraPos;
uniform vec4      lightColour;
uniform vec4      lightPosAndRadius;
uniform float	  smoothness;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
}IN;

out vec4 FragColor;

const float NoLightFactor = 0.2;
const float shadowStrenth = 0.5;

void main(){

	vec4 diffuse = vec4(1);
	vec3 normal = vec3(1);
	vec4 normalRGBA = vec4(1);
	float spec = 0;

	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);

	diffuse = texture(diffuseTex, IN.texCoord);

	normalRGBA = texture(bumpTex, IN.texCoord);
	normal = normalize(TBN * (normalRGBA.rgb * 2 - 1));

	spec = texture(specTex, IN.texCoord).r;

	float powFactor = 200.0-180*spec;

	FragColor = vec4(0);
	float shadow = 1.0f;
	if (IN.shadowProj.w > 0.0f)
	{
		shadow = clamp(textureProj(shadowTex, IN.shadowProj), shadowStrenth, 1.0);
	}

	if (lightPosAndRadius.w == 0) // Parallel light
	{
		vec3 incident = normalize(-lightPosAndRadius.xyz);
		
		float lambert = max(0.0, dot(incident.xyz, normal));
		lambert = lambert * shadow;

		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident.xyz + viewDir);

		float rFactor = max(0.0, dot(halfDir, normal));
		float sFactor = pow(rFactor, powFactor);

		vec3 colour = (diffuse.rgb * lightColour.rgb);
		colour += (lightColour.rgb * sFactor) * 0.33;

		FragColor += vec4(colour * lambert, diffuse.a);
		FragColor.rgb += (diffuse.rgb * lightColour.rgb) * NoLightFactor;
	}
	else // point light
	{
		float dist = length(lightPosAndRadius.xyz - IN.worldPos);
		float atten = 1.0 - clamp(dist / lightPosAndRadius.w, 0.0, 1.0);// farrer then closer to zero

		if (atten == 0.0)
		{
			FragColor.rgb += diffuse.rgb * NoLightFactor;
		}
		else
		{
			vec3 incident = normalize(lightPosAndRadius.xyz - IN.worldPos);
			float lambert = max(0.0, dot(incident, normal));
			lambert = lambert * shadow;

			vec3 viewDir = normalize(cameraPos - IN.worldPos);
			vec3 halfDir = normalize(incident + viewDir);

			float rFactor = max(0.0, dot(halfDir, normal));
			float sFactor = pow(rFactor, powFactor);

			vec3 colour = (diffuse.rgb * lightColour.rgb);
			colour += (lightColour.rgb * sFactor) * 0.33;
			FragColor += vec4(colour * atten * lambert, diffuse.a);
			FragColor.rgb += (diffuse.rgb * lightColour.rgb) * NoLightFactor;
		}
	}
	
	FragColor += ambient;
	vec3 toWPos = normalize(IN.worldPos - cameraPos);
	vec4 reflection = texture(cubeTex, reflect(toWPos, normalize(IN.normal)))*smoothness;
	FragColor = FragColor*(1- smoothness) + FragColor*reflection;
}

