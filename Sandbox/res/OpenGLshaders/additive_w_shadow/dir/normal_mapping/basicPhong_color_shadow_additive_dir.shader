#matUniformBlock
#size 64
#var vec4f 'amb'
#var vec4f 'diff'
#var vec4f 'spec'
#var float 'shininess'
#endMatUniformBlock

#textures
#count 1
#tex 'u_normalMap'
#endTextures

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out vec2 v_texCoord;
out vec3 v_viewPos;
out vec3 v_fragPos;
out vec4 v_fragPos_light;
out mat3 v_tbn;

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
uniform mat3 u_normalMat;

void main()
{
	//mat3 normalMat = transpose(inverse(mat3(u_modelMat)));//maybe calculate this on cpu-side, so the calculation only gets done once per model
	v_fragPos = vec3(u_modelMat * vec4(a_pos, 1.0f));
	v_fragPos_light = dirLights_v[u_lightIndex].toLightSpace * vec4(v_fragPos, 1.0f);
	v_viewPos = inverse(viewMat)[3].xyz;//try to get rid of inversing this matrix
	v_texCoord = a_texCoord;
	v_tbn = mat3(normalize(u_normalMat * a_tangent), normalize(u_normalMat * a_bitangent), normalize(u_normalMat * a_normal));//maybe we can get rid of this normalization, because we're using the normalMatrix
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

in vec2 v_texCoord;
in vec3 v_viewPos;
in vec3 v_fragPos;
in vec4 v_fragPos_light;
in mat3 v_tbn;

uniform uint u_lightIndex;//because of the usage of additive rendering, a light-index is needed
uniform sampler2D u_shadowMap;
uniform sampler2D u_normalMap;

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
	vec3 normal = texture(u_normalMap, v_texCoord).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(v_tbn * normal);

	vec3 viewDir = normalize(v_viewPos - v_fragPos);

	float shadow = 1.0f - calcDirShadow(v_fragPos_light, normal, -dirLights[u_lightIndex].direction);
	
	colOutput = calcDirLight(dirLights[u_lightIndex], normal, viewDir, shadow);

	color = vec4(colOutput, 1.0f);
};