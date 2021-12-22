#matUniformBlock
#size 0
#endMatUniformBlock

#textures
#count 3
#tex 'u_gPosition'
#tex 'u_gNormal'
#tex 'u_gAlbSpec'
#endTextures

#type vertex
#version 460 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
	v_texCoord = a_texCoord;
	gl_Position = vec4(a_pos, 0.0f, 1.0f);
};

#type fragment
#version 460 core

#define shadowBias_min 0.05f
#define BG_col vec4(0.0171f, 0.0171f, 0.0171f, 1.0f)
//for cascaded shadowMaps
#define cascade_count 3
#define zFar 100.0f
#define length_per_cascade zFar / float(cascade_count);
//for pointLight shadows
#define pointLight_zFar 60.0
#define samples 20
//for poisson sampling
#define poisson_blurr 2.0f//too much of this causes shadows to dissapear and also causes banding. Too less makes them look pretty blocky
#define poisson_samples 16
#define poisson_pixel_weight 1.0f / float(poisson_samples)
#define poisson_estimation_samples 4
#define poisson_estimation_pixel_weight 1.0f / float(poisson_estimation_samples)

layout(location = 0) out vec4 hdrColor;
layout(location = 1) out vec4 brightColor;//write a black in the second color-attachment, if bloom is dissabled

in vec2 v_texCoord;

layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;//not needed in this shader, but in the geometryPass-shader <- maybe just replace this with: mat4 padd;
	vec4 viewPos;
	mat4 padd;//this actually is the viewMat, but it is not needed in this shader at the moment
};

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gAlbSpec;
uniform sampler2DArrayShadow u_dirShadowMaps;
uniform samplerCubeArray u_pointShadowMaps;
uniform sampler2DArrayShadow u_spotShadowMaps;
uniform samplerCube u_skybox;
uniform mat4 u_invViewMat;

struct directionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec4 specular;
	mat4 toLightSpace[cascade_count];
};

struct pointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 attenuationFactors;
	mat4 toLightMatrices[6];
};

struct spotLight
{
	vec4 position;
	vec4 spotDirection;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 toLightSpace;
};

layout(std140) uniform directionalLights
{
	int dirLightCount;
	directionalLight dirLights[3];
};

layout(std140) uniform pointLights
{
	int pLightCount;
	pointLight pLights[10];
};

layout(std140) uniform spotLights
{
	int sptLightCount;
	spotLight sptLights[10];
};

vec3 calcDirLight(directionalLight light, vec3 normal, vec3 viewDir, vec3 texColor, float specularIntensity, float shininess, float ambCoefficient, float shadow)
{
	//light.direction must be normalized!!!
	//ambient
	vec3 ambient = light.ambient * (texColor * ambCoefficient);
	//diffuse
	vec3 diffuse = light.diffuse * texColor * max(0.0f, dot(-light.direction, normal)) * shadow;
	//specular
	vec3 specular = light.specular.xyz * pow(max(0.0f, dot(reflect(light.direction, normal), viewDir)), shininess) * specularIntensity * shadow;//when using"reflect()" put in the vector from the lightSource towards the fragment
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor, float specularIntensity, float shininess, float ambCoefficient, float shadow)
{
	float distance = length(fragPos - light.position.xyz);
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//Dämpfung bei weiterer Entfernung
	vec3 lightDir = normalize(light.position.xyz - fragPos);//direction from the fragment to the light
	//ambient
	vec3 ambient = light.ambient.xyz * texColor * attenuation * ambCoefficient;
	//diffuse
	vec3 diffuse = light.diffuse.xyz * texColor * max(0.0f, dot(lightDir, normal)) * attenuation * shadow;
	//specular
	vec3 specular = light.specular.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * attenuation * specularIntensity * shadow;
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor, float specularIntensity, float shininess, float ambCoefficient, float shadow)//maybe also add attenuation calculation to spotlights in order to have flashLights that fade over distance
{
	vec3 lightDir = normalize(light.position.xyz - fragPos);//normalized vector from the fragment's position towards the light
	float angle = dot(-light.spotDirection.xyz, lightDir);//cosine of the angle betwenn the vector from fragment to light and the lights spotDirection vector
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	if (angle > light.specular.w)//cuz the cosine get's bigger when the angle get's smaller(so we actually test if the angle is smaller)
	{
		//diffuse
		diffuse = light.diffuse.xyz * texColor * max(0.0f, dot(lightDir, normal)) * shadow;
		//specular
		specular = light.specular.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * specularIntensity * shadow;
	}
	//ambient-> always add the ambient component
	vec3 ambient = light.ambient.xyz * texColor * ambCoefficient;
	return (ambient + diffuse + specular);
}

vec3 sampleOffsetDirs[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

vec2 poissonDisk[16] = vec2[](
	vec2(0.310624, 0.481865),
	vec2(-0.508089, 0.157911),
	vec2(0.424457, -0.429201),
	vec2(0.493733, -0.34716),
	vec2(-0.389532, 0.670606),
	vec2(-0.630231, -0.0468335),
	vec2(0.526209, -0.582517),
	vec2(0.628677, 0.317976),
	vec2(-0.544342, 0.600365),
	vec2(-0.735574, 0.391862),
	vec2(0.283132, -0.764356),
	vec2(0.862558, -0.284717),
	vec2(0.445695, 0.951157),
	vec2(-0.719146, -0.703242),
	vec2(0.725039, -0.769394),
	vec2(0.679763, -0.669622)
);

vec2 estimationDisk[16] = vec2[](
	vec2(0.817044, 0.571409),
	vec2(-0.559821, 0.770692),
	vec2(0.504838, 0.820141),
	vec2(0.154077, -0.976351),
	vec2(-0.242354, -0.928514),
	vec2(-0.0066556, 0.977227),
	vec2(-0.0115445, -0.994106),
	vec2(0.411571, 0.901648),
	vec2(-0.968895, -0.036283),
	vec2(-0.979748, 0.165993),
	vec2(0.825836, 0.480179),
	vec2(-0.859938, 0.492169),
	vec2(0.536522, -0.837058),
	vec2(0.899093, -0.395724),
	vec2(0.96168, -0.234727),
	vec2(0.913149, -0.347998)
);

//random function is from ogldev
float random(vec3 seed, int i)//outputs random values between 0.0 and 1.0
{
	float dotProduct = dot(vec4(seed, float(i)), vec4(12.9898f, 78.233f, 45.164f, 94.673f));
	return fract(sin(dotProduct) * 43758.5453f);
}

vec2 getRandomPoisson(int i)
{
	vec2 val = poissonDisk[int(16.0f * random(vec3(gl_FragCoord.xyy), i)) % 16];
	return val;
}

vec2 getRandomEstimation(int i)
{
	vec2 val = estimationDisk[int(16.0f * random(vec3(gl_FragCoord.xyy), i)) % 16];
	return val;
}

float calcDirShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, float dirLightIndex, float cascadeIndex)
{
	vec3 clipSpaceCoords = fragPosLightSpace.xyz;
	//convert from -1 to +1 to 0 to 1 because we want to sample from the texture
	clipSpaceCoords = clipSpaceCoords * 0.5f + 0.5f;
	if (clipSpaceCoords.z > 1.0f)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	//float shadowBias = max(0.01 * (1.0f - dot(normal, lightDir)), 0.005f);
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(u_dirShadowMaps, 0).xy;//get the size of the texture at mipmap-level 0
	//following code assumes that we use GL_GEQUAL
	//estimate whether the there is any penumbra:
	for (int i = 0; i < poisson_estimation_samples; i++)
	{
		shadow += texture(u_dirShadowMaps, vec4(clipSpaceCoords.xy + getRandomEstimation(i) * texelSize, dirLightIndex * float(cascade_count) + cascadeIndex, clipSpaceCoords.z)) * poisson_estimation_pixel_weight;
	}
	if ((shadow - 1.0f) * shadow * max(dot(normal, lightDir), 0.0f) != 0.0f)//if this contition is true, we are pretty likely in penumbra
	{
		shadow *= poisson_estimation_samples;//now every sample has a weight of 1 again
		//poisson sampling:
		for (int i = 0; i < poisson_samples - poisson_estimation_samples; i++)//we do less samples here, because we already did a couple in order to estimate whether the pixel is in penumbra
		{
			shadow += texture(u_dirShadowMaps, vec4(clipSpaceCoords.xy + getRandomPoisson(i) * texelSize * poisson_blurr, dirLightIndex * float(cascade_count) + cascadeIndex, clipSpaceCoords.z));
		}
		shadow *= poisson_pixel_weight;
	}
	return shadow;
}

float calcPointShadow(vec3 fragPos, vec3 lightPos, vec3 normal, float viewDistance, float pointLightIndex)
{
	vec3 dir = fragPos - lightPos;
	float depth_current = length(dir);
	if (depth_current > pointLight_zFar)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	//float shadowBias = max(0.1 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	float shadowBias = 0.0f;
	float shadow = 0.0f;
	float diskRad = (1.0f + (viewDistance / pointLight_zFar)) / 50.0f;
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(u_pointShadowMaps, vec4(dir + sampleOffsetDirs[i] * diskRad, pointLightIndex)).r * pointLight_zFar;
		if (depth_current - shadowBias > closestDepth)
			shadow += 1.0f;
	}
	shadow /= samples;
	return shadow;
}

float calcSpotShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, float spotLightIndex)
{
	//first perform the perspective divide to get the position in clip-space(useless for othograhphic projections)
	vec3 clipSpaceCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//convert from -1 to +1 to 0 to 1 because we want to sample from the texture
	clipSpaceCoords = clipSpaceCoords * 0.5f + 0.5f;
	if (clipSpaceCoords.z > 1.0f)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	//float shadowBias = max(0.01 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	float shadowBias = 0.0f;//no shadowBias here, cuz it breaks everything
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(u_spotShadowMaps, 0).xy;//get the size of the texture at mipmap-level 0
	//following code assumes that we use GL_GEQUAL
	//estimate whether the there is any penumbra:
	for (int i = 0; i < poisson_estimation_samples; i++)
	{
		shadow += texture(u_spotShadowMaps, vec4(clipSpaceCoords.xy + getRandomEstimation(i) * texelSize, spotLightIndex, clipSpaceCoords.z)) * poisson_estimation_pixel_weight;
	}
	if ((shadow - 1.0f) * shadow * max(dot(normal, lightDir), 0.0f) != 0.0f)//if this contition is true, we are pretty likely in penumbra
	{
		shadow *= poisson_estimation_samples;//now every sample has a weight of 1 again
		//poisson sampling:
		for (int i = 0; i < poisson_samples - poisson_estimation_samples; i++)//we do less samples here, because we already did a couple in order to estimate whether the pixel is in penumbra
		{
			shadow += texture(u_spotShadowMaps, vec4(clipSpaceCoords.xy + getRandomPoisson(i) * texelSize * poisson_blurr, spotLightIndex, clipSpaceCoords.z));
		}
		shadow *= poisson_pixel_weight;
	}
	return shadow;
}

vec4 getPosition(float zViewSpace, vec2 uv)
{
	vec4 result;
	float zNear = 0.1f;//zFar is already defined
	vec2 clipSpace = (uv * 2.0f) - 1.0f;
	result.x = -(clipSpace.x * zViewSpace/*times tan of half horizontal fov*/);
	result.y = -(clipSpace.y * zViewSpace * (1080.0f / 1920.0f)/*this value is equal to the tangent of the half vertical fov, or just the aspect ratio(height/width)*/);
	result.z = zViewSpace;
	result.w = 1.0f;
	return result;
}

vec4 convertRGBE(vec4 rgbe)
{
	rgbe *= 255.0f;
	//convert from rgbe to hdr rgba
	float factor = pow(2.0f, rgbe.a - 128.0f) / 256.0f;
	return vec4(rgbe.rgb * factor, 1.0f);
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	vec4 posTexRead = texture(u_gPosition, v_texCoord).rgba;
	float reflectiveCoefficient = posTexRead.x;
	float fragZView = posTexRead.z;
	float ambCoefficient = posTexRead.w;
	vec4 fragPos = u_invViewMat * getPosition(fragZView, v_texCoord);//restore viewSpacePosition and transform to worldSpace
	vec4 AlbSpec = texture(u_gAlbSpec, v_texCoord).rgba;
	float specIntensity = AlbSpec.a;
	//first restore the normal's z-component and then restore its sign with the sign of the shininess-value
	vec4 normal = texture(u_gNormal, v_texCoord).rgba;
	float emissiveMultiplier = normal.z;//<-retrieve the emissiveMultiplier from the texture's z-component
	normal.z = sign(normal.w) * sqrt(1.0f - (normal.x * normal.x) - (normal.y * normal.y));
	float shininess = abs(normal.w);//<-retrieve the shininess from the normal's w-component -> using abs, because this carries the sign of the normal's z-component#
	normal.w = 0.0f;
	normal = u_invViewMat * normal;
	vec3 viewDir = viewPos.xyz - fragPos.xyz;
	float viewDistance = length(viewDir);
	viewDir = normalize(viewDir);

	brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (normal.xyz != vec3(0.0f, 0.0f, 0.0f))
	{
		uint cascadeIndex = 0;
		for (uint i = 1; i < cascade_count; i++)
		{
			float length = float(i) * length_per_cascade;
			if ((-fragZView) > length)//note that this uses the length of the vector from camera to pixel instead of the depth -> maybe change this
				cascadeIndex++;
		}
		for (int i = 0; i < dirLightCount; i++)
		{
			float shadow = 1.0f - calcDirShadow(dirLights[i].toLightSpace[cascadeIndex] * fragPos, normal.xyz, -dirLights[i].direction, float(i), float(cascadeIndex));
			colOutput += calcDirLight(dirLights[i], normal.xyz, viewDir, AlbSpec.xyz, specIntensity, shininess, ambCoefficient, shadow);
		}

		for (int i = 0; i < pLightCount; i++)
		{
			float shadow = 1.0f - calcPointShadow(fragPos.xyz, pLights[i].position.xyz, normal.xyz, viewDistance, float(i));
			colOutput += calcPointLight(pLights[i], normal.xyz, fragPos.xyz, viewDir, AlbSpec.xyz, specIntensity, shininess, ambCoefficient, shadow);
		}

		for (int i = 0; i < sptLightCount; i++)
		{
			float shadow = 1.0f - calcSpotShadow(sptLights[i].toLightSpace * fragPos, normal.xyz, normalize(sptLights[i].position.xyz - fragPos.xyz), float(i));
			colOutput += calcSpotLight(sptLights[i], normal.xyz, fragPos.xyz, viewDir, AlbSpec.xyz, specIntensity, shininess, ambCoefficient, shadow);
		}
		//apply emissive parameter:
		colOutput += AlbSpec.xyz * emissiveMultiplier;
		//apply reflective parameter:
		colOutput += convertRGBE(texture(u_skybox, reflect(-viewDir, normal.xyz))).xyz * reflectiveCoefficient;

		hdrColor = vec4(colOutput, 1.0f);
	}
	else
	{
		hdrColor = BG_col;
	}
};