#matUniformBlock
#size 0
#endMatUniformBlock

#textures
#count 1
#tex 'u_texture'
#endTextures

#type vertex
#version 460 core

//ONLY FOR RGB-TEXTURES, NO RGBA

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out vec3 v_normal;
out vec3 v_viewPos;//maybe make this a uniform(or a uniformBuffer) that is accepted by the fragment-shader->we are calulating this per vertex right now, which is inefficient because it only needs to be calculated
//once per model
out vec3 v_fragPos;
out vec2 v_texCoord;

layout(std140) uniform ViewProjection
{
	mat4 viewMat;
	mat4 projMat;
};

uniform mat4 u_modelMat;

void main()
{
	mat3 normalMat = transpose(inverse(mat3(u_modelMat)));//maybe calculate this on cpu-side, so the calculation only gets done once per model
	v_normal = normalMat * a_normal;//try to get rid of this normalization->unfortunately have to normalize in fragmentShader
	v_fragPos = (u_modelMat * vec4(a_pos, 1.0f)).xyz;
	v_viewPos = inverse(viewMat)[3].xyz;//try to get rid of inversing this matrix
	v_texCoord = a_texCoord;
	gl_Position = projMat * viewMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

#define AMB_coefficient 0.1f
#define SPEC_coefficient 1.5f
#define shininess 100.0f

#define shadowBias_min 0.005f
#define zFar 60.0f

struct pointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 attenuationFactors;
	mat4 toLightMatrices[6];
};

layout(std140) uniform pointLights
{
	int pLightCount;
	pointLight pLights[10];
};

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_viewPos;
in vec3 v_fragPos;
in vec2 v_texCoord;

uniform uint u_lightIndex;
uniform sampler2D u_texture;
uniform samplerCube u_shadowMap;

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor, float shadow)
{
	float distance = length(fragPos - light.position.xyz);
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//Dämpfung bei weiterer Entfernung
	vec3 lightDir = normalize(light.position.xyz - fragPos);//direction from the fragment to the light
	//ambient
	vec3 ambient = light.ambient.xyz * texColor * attenuation * AMB_coefficient;
	//diffuse
	vec3 diffuse = light.diffuse.xyz * texColor * max(0.0f, dot(lightDir, normal)) * attenuation * shadow;
	//specular
	vec3 specular = light.specular.xyz * texColor * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * attenuation * SPEC_coefficient * shadow;
	return (ambient + diffuse + specular);
}

float calcPointShadow(vec3 lightPos, vec3 normal, vec3 lightDir)
{
	vec3 dir = v_fragPos - lightPos;
	float closestDepth = texture(u_shadowMap, dir).r * zFar;
	float depth_current = length(dir);
	if (depth_current > zFar)
		return 0.0f;//no shadow if we are past the far-plane of the light's camera
	float shadowBias = max(0.1 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	//float shadowBias = 1.1f;
	float shadow = depth_current - shadowBias > closestDepth ? 1.0f : 0.0f;//if the current depth is higher than the sampled value from the texture the pixel is in shadow
	return shadow;
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	vec3 col_interpolate = texture(u_texture, v_texCoord).xyz;
	vec3 normal = normalize(v_normal);//normalize because normals can get shorter when interpolating

	float shadow = 1.0f - calcPointShadow(pLights[u_lightIndex].position.xyz, normal, normalize(pLights[u_lightIndex].position.xyz - v_fragPos));
	//shadow = 1.0f;
	colOutput = calcPointLight(pLights[u_lightIndex], normal, v_fragPos, viewDir, col_interpolate, shadow);
	//colOutput = vec3(texture(u_shadowMap, v_fragPos - pLights[u_lightIndex].position.xyz));

	color = vec4(colOutput, 1.0f);
};