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
out vec3 v_viewPos;
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

struct directionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct pointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 attenuationFactors;
};

struct spotLight
{
	vec3 position;
	vec3 spotDirection;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutOff;//cosine of the cutOffAngle
};

layout(std140) uniform directionalLights
{
	int dirLightCount;
	directionalLight dirLights[10];
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

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_viewPos;
in vec3 v_fragPos;
in vec2 v_texCoord;

uniform sampler2D u_texture;

vec3 calcDirLight(directionalLight light, vec3 normal, vec3 viewDir, vec3 texColor)
{
	//light.direction must be normalized!!!
	//ambient
	vec3 ambient = light.ambient * (texColor * AMB_coefficient);
	//diffuse
	vec3 diffuse = light.diffuse * texColor * max(0.0f, dot(-light.direction, normal));
	//specular
	vec3 specular = light.specular * texColor * pow(max(0.0f, dot(reflect(light.direction, normal), viewDir)), shininess) * SPEC_coefficient;//when using"reflect()" put in the vector from the lightSource towards the fragment
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor)
{
	float distance = length(fragPos - light.position);
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//Dämpfung bei weiterer Entfernung
	vec3 lightDir = normalize(light.position - fragPos);//direction from the fragment to the light
	//ambient
	vec3 ambient = light.ambient * texColor * attenuation * AMB_coefficient;
	//diffuse
	vec3 diffuse = light.diffuse * texColor * max(0.0f, dot(lightDir, normal)) * attenuation;
	//specular
	vec3 specular = light.specular * texColor * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * attenuation * SPEC_coefficient;
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor)//maybe also add attenuation calculation to spotlights in order to have flashLights that fade over distance
{
	vec3 lightDir = normalize(light.position - fragPos);//normalized vector from the fragment's position towards the light
	float angle = dot(-light.spotDirection , lightDir);//cosine of the angle betwenn the vector from fragment to light and the lights spotDirection vector
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	if (angle > light.cutOff)//cuz the cosine get's bigger when the angle get's smaller(so we actually test if the angle is smaller)
	{
		//diffuse
		diffuse = light.diffuse * texColor * max(0.0f, dot(lightDir, normal));
		//specular
		specular = light.specular * texColor * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * SPEC_coefficient;
	}
	//ambient-> always add the ambient component
	vec3 ambient = light.ambient * texColor * AMB_coefficient;
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	
	vec3 col_interpolate = texture(u_texture, v_texCoord).xyz;
	vec3 normal = normalize(v_normal);//normalize because normals can get shorter when interpolating
	
	for (int i = 0; i < dirLightCount; i++)
	{
		colOutput += calcDirLight(dirLights[i], normal, viewDir, col_interpolate);
	}

	for (int i = 0; i < pLightCount; i++)
	{
		colOutput += calcPointLight(pLights[i], normal, v_fragPos, viewDir, col_interpolate);
	}

	for (int i = 0; i < sptLightCount; i++)
	{
		colOutput += calcSpotLight(sptLights[i], normal, v_fragPos, viewDir, col_interpolate);
	}

	color = vec4(colOutput, 1.0f);
};