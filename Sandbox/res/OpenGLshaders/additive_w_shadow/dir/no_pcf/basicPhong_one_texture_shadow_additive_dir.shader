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
out vec4 v_fragPos_light;
out vec2 v_texCoord;

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
	v_normal = normalMat * a_normal;//try to get rid of this normalization->unfortunately have to normalize in fragmentShader
	v_fragPos = (u_modelMat * vec4(a_pos, 1.0f)).xyz;
	v_fragPos_light = dirLights_v[u_lightIndex].toLightSpace * vec4(v_fragPos, 1.0f);
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

struct directionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec4 specular;
	mat4 toLightSpace;
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
in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_shadowMap;
uniform uint u_lightIndex;//since were using multipass-rendering, we need the light-index <- index into the array of directional lights

vec3 calcDirLight(directionalLight light, vec3 normal, vec3 viewDir, vec3 texColor, float shadow)
{
	//light.direction must be normalized!!!
	//ambient
	vec3 ambient = light.ambient * (texColor * AMB_coefficient);
	//diffuse
	vec3 diffuse = light.diffuse * texColor * max(0.0f, dot(-light.direction, normal)) * shadow;
	//specular
	vec3 specular = light.specular.xyz * texColor * pow(max(0.0f, dot(reflect(light.direction, normal), viewDir)), shininess) * SPEC_coefficient * shadow;//when using"reflect()" put in the vector from the lightSource towards the fragment
	return (ambient + diffuse + specular);
}

float calcDirShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	//first perform the perspective divide to get the position in clip-space(useless for othograhphic projections)
	vec3 clipSpaceCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//convert from -1 to +1 to 0 to 1 because we want to sample from the texture
	clipSpaceCoords = clipSpaceCoords * 0.5f + 0.5f;
	float closestDepth = texture(u_shadowMap, clipSpaceCoords.xy).r;
	float shadowBias = max(0.01 * (1.0f - dot(normal, lightDir)), shadowBias_min);
	//float shadowBias = 0.0f;
	float shadow = clipSpaceCoords.z - shadowBias > closestDepth ? 1.0f : 0.0f;//if the current depth(projCoords.z) is higher than the sampled value from the texture the pixel is in shadow
	if (clipSpaceCoords.z > 1.0f)
		shadow = 0.0f;//no shadow if we are outside of the viewing-frustum of the light
	return shadow;
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	
	vec3 col_interpolate = texture(u_texture, v_texCoord).xyz;
	vec3 normal = normalize(v_normal);//normalize because normals can get shorter when interpolating

	float shadow = 1.0f - calcDirShadow(v_fragPos_light, normal, -dirLights[u_lightIndex].direction);
	
	colOutput = calcDirLight(dirLights[u_lightIndex], normal, viewDir, col_interpolate, shadow);

	color = vec4(colOutput, 1.0f);
};