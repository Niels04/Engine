#matUniformBlock
#size 64
#var vec4f 'amb'
#var vec4f 'diff'
#var vec4f 'spec'
#var float 'shininess'
#endMatUniformBlock

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out vec3 v_normal;
out vec3 v_viewPos;
out vec3 v_fragPos;
out vec4 v_fragPos_light;

layout(std140) uniform ViewProjection
{
	mat4 viewMat;
	mat4 projMat;
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

layout(std140) uniform spotLights_v
{
	int sptLightCount_v;
	spotLight sptLights_v[10];
};

uniform uint u_lightIndex;
uniform mat4 u_modelMat;
uniform mat3 u_normalMat;

void main()
{
	v_normal = u_normalMat * a_normal;
	v_fragPos = vec3(u_modelMat * vec4(a_pos, 1.0f));
	v_fragPos_light = sptLights_v[u_lightIndex].toLightSpace * vec4(v_fragPos, 1.0f);
	v_viewPos = inverse(viewMat)[3].xyz;//try to get rid of inversing this matrix
	gl_Position = projMat * viewMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

#define shadowBias_min 0.005f

struct spotLight
{
	vec4 position;
	vec4 spotDirection;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 padd;
	mat4 toLightSpace;
};

layout(std140) uniform material
{
	vec4 amb;
	vec4 diff;
	vec4 spec;
	float shininess;
};

layout(std140) uniform spotLights
{
	int sptLightCount;
	spotLight sptLights[10];
};

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_viewPos;
in vec3 v_fragPos;
in vec4 v_fragPos_light;

uniform uint u_lightIndex;//because of the usage of additive rendering, a light-index is needed
uniform sampler2D u_shadowMap;

vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)//maybe also add attenuation calculation to spotlights in order to have flashLights that fade over distance
{
	vec3 lightDir = normalize(light.position.xyz - fragPos);//normalized vector from the fragment's position towards the light
	float angle = dot(-light.spotDirection.xyz , lightDir);//cosine of the angle betwenn the vector from fragment to light and the lights spotDirection vector
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	if (angle > light.specular.w)//cuz the cosine get's bigger when the angle get's smaller(so we actually test if the angle is smaller)
	{
		//diffuses
		diffuse = light.diffuse.xyz * diff.xyz * max(0.0f, dot(lightDir, normal)) * shadow;
		//specular
		specular = light.specular.xyz * spec.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * shadow;
	}
	//ambient-> always add the ambient component
	vec3 ambient = light.ambient.xyz * amb.xyz;

	return (ambient + diffuse + specular);
}

float calcSpotShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	//first perform the perspective divide to get the position in clip-space(useless for othograhphic projections)
	vec3 clipSpaceCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//convert from -1 to +1 to 0 to 1 because we want to sample from the texture
	clipSpaceCoords = clipSpaceCoords * 0.5f + 0.5f;
	if (clipSpaceCoords.z > 1.0f)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	//float shadowBias = max(0.01 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	float shadowBias = 0.0f;
	//pcf:
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(u_shadowMap, 0);//get the size of the texture at mipmap-level 0
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_shadowMap, clipSpaceCoords.xy + vec2(x, y) * texelSize).r;
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
	
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	vec3 normal = normalize(v_normal);

	float shadow = 1.0f - calcSpotShadow(v_fragPos_light, normal, normalize(sptLights[u_lightIndex].position.xyz - v_fragPos));

	colOutput = calcSpotLight(sptLights[u_lightIndex], normal, v_fragPos, viewDir, shadow);

	color = vec4(colOutput, 1.0f);
};