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
out mat3 v_tbn;

layout(std140) uniform ViewProjection
{
	mat4 viewMat;
	mat4 projMat;
};

uniform mat4 u_modelMat;
uniform mat3 u_normalMat;

void main()
{
	//mat3 normalMat = transpose(inverse(mat3(u_modelMat)));//maybe calculate this on cpu-side, so the calculation only gets done once per model
	v_fragPos = vec3(u_modelMat * vec4(a_pos, 1.0f));
	v_viewPos = inverse(viewMat)[3].xyz;//try to get rid of inversing this matrix
	v_texCoord = a_texCoord;
	v_tbn = mat3(normalize(u_normalMat * a_tangent), normalize(u_normalMat * a_bitangent), normalize(u_normalMat * a_normal));//maybe we can get rid of this normalization, because we're using the normalMatrix
	gl_Position = projMat * viewMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

#define shadowBias_min 0.005f
#define zFar 60.0
#define samples 20

struct pointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 attenuationFactors;
	mat4 toLightMatrices[6];
};

layout(std140) uniform material
{
	vec4 amb;
	vec4 diff;
	vec4 spec;
	float shininess;
};

layout(std140) uniform pointLights
{
	int pLightCount;
	pointLight pLights[10];
};

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 bright_color;

in vec2 v_texCoord;
in vec3 v_viewPos;
in vec3 v_fragPos;
in mat3 v_tbn;

uniform uint u_lightIndex;//because of the usage of additive rendering, a light-index is needed
uniform samplerCube u_shadowMap;
uniform sampler2D u_normalMap;

vec3 sampleOffsetDirs[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
{
	float distance = length(fragPos - light.position.xyz);
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//D�mpfung bei weiterer Entfernung
	vec3 lightDir = normalize(light.position.xyz - fragPos);//direction from the fragment to the light
	//ambient
	vec3 ambient = light.ambient.xyz * amb.xyz * attenuation;
	//diffuse
	vec3 diffuse = light.diffuse.xyz * diff.xyz * max(0.0f, dot(lightDir, normal)) * attenuation * shadow;
	//specular
	vec3 specular = light.specular.xyz * spec.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * attenuation * shadow;
	return (ambient + diffuse + specular);
}

float calcPointShadow(vec3 lightPos, vec3 normal, vec3 lightDir, float viewDistance)
{
	vec3 dir = v_fragPos - lightPos;
	float depth_current = length(dir);
	if (depth_current > zFar)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	float shadowBias = max(0.1 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	float shadow = 0.0f;
	float diskRad = (1.0f + (viewDistance / zFar)) / 50.0f;
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(u_shadowMap, dir + sampleOffsetDirs[i] * diskRad).r * zFar;
		if (depth_current - shadowBias > closestDepth)
			shadow += 1.0f;
	}
	shadow /= samples;
	return shadow;
}

void main()
{
	//get the LIGHT POSITION BY THE INDEX INTO THE LIGH-ARRAY
	vec3 colOutput = vec3(0.0f);
	
	float viewDistance = length(v_viewPos - v_fragPos);
	vec3 viewDir = (v_viewPos - v_fragPos) / viewDistance;
	vec3 normal = texture(u_normalMap, v_texCoord).rgb;
	normal = normal * 2.0f - 1.0f;//think about if this really needs to be normalized here
	normal = normalize(v_tbn * normal);

	float shadow = 1.0f - calcPointShadow(pLights[u_lightIndex].position.xyz, normal, normalize(pLights[u_lightIndex].position.xyz - v_fragPos), viewDistance);
	//shadow = 1.0f;
	colOutput = calcPointLight(pLights[u_lightIndex], normal, v_fragPos, viewDir, shadow);
	//colOutput = vec3(texture(u_shadowMap, v_fragPos - pLights[u_lightIndex].position.xyz));

	color = vec4(colOutput, 1.0f);
	bright_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);//if bloom is enabled, the second color-attachment needs to be set to black, in order for this shader not to contribute to the bloom-effect
};