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

layout(location = 0) out vec4 hdrColor;
layout(location = 1) out vec4 brightColor;//write a black in the second color-attachment, if bloom is dissabled

in vec2 v_texCoord;

layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;//not needed in this shader, but in the geometryPass-shader <- maybe just replace this with: mat4 padd;
	vec4 viewPos;
};

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gAlbSpec;
uniform sampler2DArray u_dirShadowMaps;
uniform samplerCubeArray u_pointShadowMaps;
uniform sampler2DArray u_spotShadowMaps;
uniform mat4 u_viewMat;//viewMat that is needed to find out the depth of each pixel in order to determine the correct cascade for cascaded shadowMaps

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
	vec4 pad;
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
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//D�mpfung bei weiterer Entfernung
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

float calcDirShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, float dirLightIndex, float cascadeIndex)
{
	vec3 clipSpaceCoords = fragPosLightSpace.xyz;
	//convert from -1 to +1 to 0 to 1 because we want to sample from the texture
	clipSpaceCoords = clipSpaceCoords * 0.5f + 0.5f;
	if (clipSpaceCoords.z > 1.0f)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	//float shadowBias = max(0.01 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	float shadowBias = 0.0f;
	//pcf:
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(u_dirShadowMaps, 0).xy;//get the size of the texture at mipmap-level 0
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_dirShadowMaps, vec3(clipSpaceCoords.xy + vec2(x, y) * texelSize, dirLightIndex * float(cascade_count) + cascadeIndex)).r;
			shadow += clipSpaceCoords.z - shadowBias > pcfDepth ? 1.0f : 0.0f;//if the current depth(projCoords.z) is higher than the sampled value from the texture the pixel is in shadow
		}
	}
	shadow /= 9.0f;
	//end pcf
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
	//pcf:
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(u_spotShadowMaps, 0).xy;//get the size of the texture at mipmap-level 0
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_spotShadowMaps, vec3(clipSpaceCoords.xy + vec2(x, y) * texelSize, spotLightIndex)).r;
			shadow += clipSpaceCoords.z - shadowBias > pcfDepth ? 1.0f : 0.0f;//if the current depth(projCoords.z) is higher than the sampled value from the texture the pixel is in shadow
		}
	}
	shadow /= 9.0f;
	//end pcf
	return shadow;
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	vec4 fragPos = texture(u_gPosition, v_texCoord).rgba;
	float ambCoefficient = fragPos.w;
	fragPos.w = 1.0f;
	vec4 AlbSpec = texture(u_gAlbSpec, v_texCoord).rgba;
	float specIntensity = AlbSpec.a;
	vec4 normal = texture(u_gNormal, v_texCoord).rgba;
	float shininess = normal.w;//<-retrieve the shininess from the normal's w-component
	vec3 viewDir = viewPos.xyz - fragPos.xyz;
	float viewDistance = length(viewDir);
	viewDir = normalize(viewDir);

	brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (normal.xyz != vec3(0.0f, 0.0f, 0.0f))
	{
		uint cascadeIndex = 0;
		float worldSpaceDepth = -(u_viewMat * fragPos).z;
		for (uint i = 1; i < cascade_count; i++)
		{
			float length = float(i) * length_per_cascade;
			if (worldSpaceDepth > length)//note that this uses the length of the vector from camera to pixel instead of the depth -> maybe change this
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

		hdrColor = vec4(colOutput, 1.0f);
	}
	else
	{
		hdrColor = BG_col;
	}
};