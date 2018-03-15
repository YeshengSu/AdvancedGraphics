# version 150 core

uniform sampler2DShadow shadowTex0;
uniform sampler2DShadow shadowTex1;

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D specTex;

uniform vec4      ambient;
uniform vec3      cameraPos;
uniform vec4      lightColour[2];
uniform vec4      lightPosAndRadius[2];

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj[2];
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
	for (int i = 0; i < 2; i++)
	{
		if (lightPosAndRadius[i].w == 0) // Parallel light
		{
			if (i == 0)
			{
				if (IN.shadowProj[0].w > 0.0f)
				{
					shadow *= clamp(textureProj(shadowTex0, IN.shadowProj[0]), shadowStrenth*1.5, 1.0);
				}
			}

			vec3 incident = normalize(-lightPosAndRadius[i].xyz);
			float lambert = max(0.0, dot(incident.xyz, normal));
			//lambert = lambert * shadow;

			vec3 viewDir = normalize(cameraPos - IN.worldPos);
			vec3 halfDir = normalize(incident.xyz + viewDir);

			float rFactor = max(0.0, dot(halfDir, normal));
			float sFactor = pow(rFactor, powFactor);

			vec3 colour = (diffuse.rgb * lightColour[i].rgb);
			colour += (lightColour[i].rgb * sFactor) * 0.33;
			FragColor += vec4(colour * lambert, diffuse.a);
			FragColor.rgb += (diffuse.rgb * lightColour[i].rgb) * NoLightFactor;
		}
		else // point light
		{
			float dist = length(lightPosAndRadius[i].xyz - IN.worldPos);
			float atten = 1.0 - clamp(dist / lightPosAndRadius[i].w, 0.0, 1.0);// farrer then closer to zero

			if (i == 1)
			{
				if (IN.shadowProj[1].w > 0.0f)
				{
					float tempshadow = shadow;
					tempshadow *= clamp(textureProj(shadowTex1, IN.shadowProj[1]), shadowStrenth, 1.0);
					
					shadow *= mix(tempshadow, 1, pow(1-atten,5));
				}
			}
			if (atten == 0.0)
			{
				FragColor.rgb += diffuse.rgb * NoLightFactor;
			}
			else
			{
				vec3 incident = normalize(lightPosAndRadius[i].xyz - IN.worldPos);
				float lambert = max(0.0, dot(incident, normal));
				//lambert = lambert * shadow;

				vec3 viewDir = normalize(cameraPos - IN.worldPos);
				vec3 halfDir = normalize(incident + viewDir);

				float rFactor = max(0.0, dot(halfDir, normal));
				float sFactor = pow(rFactor, powFactor);

				vec3 colour = (diffuse.rgb * lightColour[i].rgb);
				colour += (lightColour[i].rgb * sFactor) * 0.33;
				FragColor += vec4(colour * atten * lambert, diffuse.a);
				FragColor.rgb += (diffuse.rgb * lightColour[i].rgb) * NoLightFactor;
			}
		}
	}
	FragColor += ambient;
	FragColor *= shadow;
}

