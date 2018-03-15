# version 150 core

uniform sampler2D		reflectionTexture;
uniform sampler2D		refractionTexture;
uniform sampler2D		dudvMap;
uniform sampler2D		dumpMap;
uniform sampler2D		depthMap;
uniform float			farPlane;
uniform float			nearPlane;
uniform float			moveFactor;
uniform vec3			cameraPos;
uniform vec4			ambient;
uniform vec4			lightColour;
uniform vec4			lightPosAndRadius;

in Vertex {
	vec4 clipSpace; // use to find out uv according to screen xy
	vec4 colour;
	vec3 worldPos;
	vec2 texCoord;
} IN ;

out vec4 FragColor;

const float waveStrength = 0.02;
const vec4 waterColor = vec4(0.0f, 0.3f, 0.5f, 1.0f);
const float reflectiveFactor = 1.5f;
const float shineDamper = 0.6f;
const float reflectivity = 0.6f;
const float powFactor = 10.0f;
const float NoLightFactor = 0.1f;
void main ( void ) {
	//make it from [-1,1] to [0,1] screen space
	vec2 ndc = ((IN.clipSpace.xy / IN.clipSpace.w) * 0.5) + 0.5;
	
	vec2 distortedTexCoords = texture(dudvMap, vec2(IN.texCoord.x + moveFactor, IN.texCoord.y)).rg*0.1;
	distortedTexCoords = IN.texCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

	// sample reflection tex
	vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y); 
	reflectionTexCoord += totalDistortion;
	reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
	reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);
	vec4 reflectionColour = texture(reflectionTexture, reflectionTexCoord);

	// sample refraction tex
	vec2 refractionTexCoord = vec2(ndc.x, ndc.y);
	refractionTexCoord += totalDistortion;
	refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);
	vec4 refractionColour = texture(refractionTexture, refractionTexCoord);

	// culculate depth of water
	float depth = texture(depthMap, refractionTexCoord).r;
	float floorDistance = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
	depth = gl_FragCoord.z;
	float waterDistance = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
	float waterDepth = floorDistance - waterDistance;

	// culculate refractive rate
	vec3 toCamera = normalize(cameraPos - IN.worldPos);
	float refractiveFactor = dot(toCamera, vec3(0.0f, 1.0f, 0.0f));
	refractiveFactor = pow(refractiveFactor, reflectiveFactor);

	// get final color of water
	vec4 waterFinalColor;
	waterFinalColor = mix(reflectionColour, refractionColour, refractiveFactor);
	waterFinalColor = mix(waterFinalColor, waterColor, 0.2f);

	//lighting culculation
	vec4 lightFinalColor;
	vec4 normalColor = texture(dumpMap, distortedTexCoords);
	vec3 normal = (normalColor.rgb * 2 - 1);
	normal = normalize(normal);
	if (lightPosAndRadius.w == 0) // Parallel light
	{
		vec3 incident = normalize(-lightPosAndRadius.xyz);
		float lambert = max(0.0, dot(incident, normal)) * shineDamper;


		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float rFactor = max(0.0, dot(halfDir, normal));
		float sFactor = pow(rFactor, powFactor);

		vec3 colour = (waterFinalColor.rgb * lightColour.rgb);
		colour += (lightColour.rgb * sFactor) * 0.33;
		lightFinalColor = vec4(colour * lambert, 1.0f);
		lightFinalColor.rgb += (waterFinalColor.rgb * lightColour.rgb) * NoLightFactor;
		lightFinalColor += ambient;
	}
	else // point light
	{
		float dist = length(lightPosAndRadius.xyz - IN.worldPos);
		float atten = 1.0 - clamp(dist / lightPosAndRadius.w, 0.0, 1.0);

		if (atten == 0.0)
		{
			lightFinalColor.rgb = waterFinalColor.rgb * NoLightFactor;
			lightFinalColor += ambient;
			FragColor = lightFinalColor + waterFinalColor;
			return;
		}

		vec3 incident = normalize(lightPosAndRadius.xyz - IN.worldPos);
		float lambert = max(0.0, dot(incident, normal)) * shineDamper;


		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float rFactor = max(0.0, dot(halfDir, normal));
		float sFactor = pow(rFactor, powFactor);

		vec3 colour = (waterFinalColor.rgb * lightColour.rgb);
		colour += (lightColour.rgb * sFactor) * 0.33;
		lightFinalColor = vec4(colour * atten * lambert, waterFinalColor.a);
		lightFinalColor.rgb += (waterFinalColor.rgb * lightColour.rgb) * NoLightFactor;
		lightFinalColor += ambient;
	}

	FragColor = waterFinalColor + (lightFinalColor);
	FragColor.a = clamp(waterDepth / 100000, 0.0f, 1.0f);
}
