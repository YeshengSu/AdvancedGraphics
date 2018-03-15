# version 150 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform vec4  lightPosRaidus;
uniform vec4  lightColour;

in mat4 inverseProjView;
out vec4 FragColor[2];
//
void main(void)
{
	vec3 pos = vec3((gl_FragCoord.x * pixelSize.x), (gl_FragCoord.y * pixelSize.y), 0.0f);

	pos.z = texture(depthTex,pos.xy).r;

	vec4 normTexInfo = texture(normTex, pos.xy);
	vec3 normal		 = normalize(normTexInfo.xyz * 2.0 + 1.0);

	vec4 clip		 = inverseProjView * vec4(pos * 2.0f - 1.0f, 1.0);
	pos				 = clip.xyz / clip.w;
					 
	float dist		 = length(lightPosRaidus.xyz - pos);
	float atten		 = 1.0 - clamp(dist / lightPosRaidus.w, 0.0, 1.0);

	if (atten == 0.0)
	{
		discard;
	}

	float powFactor = 200.0 - 180 * normTexInfo.w;

	vec3 incident = normalize(lightPosRaidus.xyz - pos);
	vec3 viewDir = normalize(cameraPos - pos);
	vec3 halpDir = normalize(incident + viewDir);

	float lambert = clamp(dot(incident, normal), 0.0f, 1.0f);
	float rFactor = clamp(dot(halpDir, normal), 0.0f, 1.0f);
	float sFactor = pow(rFactor, powFactor);

	FragColor[0] = vec4(lightColour.xyz * lambert * atten, 1.0);
	FragColor[1] = vec4(lightColour.xyz * sFactor * atten * 0.33, 1.0);

}
