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

struct directionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec4 specular;
	mat4 toLightSpace;
};

layout(std140) uniform directionalLights_v
{
	int dirLightCount_v;
	directionalLight dirLights_v[10];
};

uniform uint u_lightIndex;
uniform mat4 u_modelMat;

void main()
{
	mat3 normalMat = transpose(inverse(mat3(u_modelMat)));//maybe calculate this on cpu-side, so the calculation only gets done once per model
	v_normal = normalMat * a_normal;
	v_fragPos = vec3(u_modelMat * vec4(a_pos, 1.0f));
	v_fragPos_light = dirLights_v[u_lightIndex].toLightSpace * vec4(v_fragPos, 1.0f);
	v_viewPos = inverse(viewMat)[3].xyz;//try to get rid of inversing this matrix
	gl_Position = projMat * viewMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

#define shadowBias_min 0.005f

struct directionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec4 specular;
	mat4 toLightSpace;
};

layout(std140) uniform material
{
	vec4 amb;
	vec4 diff;
	vec4 spec;
	float shininess;
};

layout(std140) uniform directionalLights
{
	int dirLightCount;
	directionalLight dirLights[10];
};

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_viewPos;
in vec3 v_fragPos;
in vec4 v_fragPos_light;

uniform uint u_lightIndex;//because of the usage of additive rendering, a light-index is needed
uniform sampler2D u_shadowMap;

vec3 calcDirLight(directionalLight light, vec3 normal, vec3 viewDir, float shadow)
{
	//light.direction must be normalized!!!
	//ambient
	vec3 ambient = light.ambient * amb.xyz;
	//diffuse
	vec3 diffuse = light.diffuse * diff.xyz * max(0.0f, dot(-light.direction.xyz, normal)) * shadow;
	//specular
	vec3 specular = light.specular.xyz * spec.xyz * pow(max(0.0f, dot(reflect(light.direction, normal), viewDir)), shininess) * shadow;//when using"reflect()" put in the vector form the lightSource towards the fragment
	return (ambient + diffuse + specular);
}

float calcDirShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 clipSpaceCoords = fragPosLightSpace.xyz;
	//convert from -1 to +1 to 0 to 1 because we want to sample from the texture
	clipSpaceCoords = clipSpaceCoords * 0.5f + 0.5f;
	if (clipSpaceCoords.z > 1.0f)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	float shadowBias = max(0.01 * (1.0f - dot(normal, lightDir)), shadowBias_min);
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

	float shadow = 1.0f - calcDirShadow(v_fragPos_light, normal, -dirLights[u_lightIndex].direction);
	
	colOutput = calcDirLight(dirLights[u_lightIndex], normal, viewDir, shadow);

	color = vec4(colOutput, 1.0f);
};