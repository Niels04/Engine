#matUniformBlock
#size 52
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

layout(std140) uniform ViewProjection
{
	mat4 viewMat;
	mat4 projMat;
};

uniform mat4 u_modelMat;

void main()
{
	mat3 normalMat = transpose(inverse(mat3(u_modelMat)));//maybe calculate this on cpu-side, so the calculation only gets done once per model
	v_normal = normalMat * a_normal;
	v_fragPos = (u_modelMat * vec4(a_pos, 1.0f)).xyz;
	v_viewPos = inverse(viewMat)[3].xyz;//try to get rid of inversing this matrix
	gl_Position = projMat * viewMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

struct directionalLight
{
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct pointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuationFactors;
};

struct spotLight
{
	vec4 position;
	vec4 spotDirection;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float cutOff;//cosine of the cutOffAngle
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

vec3 calcDirLight(directionalLight light, vec3 normal, vec3 viewDir)
{
	//light.direction must be normalized!!!
	//ambient
	vec3 ambient = light.ambient.xyz * amb.xyz;
	//diffuse
	vec3 diffuse = light.diffuse.xyz * diff.xyz * max(0.0f, dot(-light.direction.xyz, normal));
	//specular
	vec3 specular = light.specular.xyz * spec.xyz * pow(max(0.0f, dot(reflect(light.direction.xyz, normal), viewDir)), shininess);//when using"reflect()" put in the vector form the lightSource towards the fragment
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float distance = length(fragPos - light.position.xyz);
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//Dämpfung bei weiterer Entfernung
	vec3 lightDir = normalize(light.position.xyz - fragPos);//direction from the fragment to the light
	//ambient
	vec3 ambient = light.ambient.xyz * amb.xyz * attenuation;
	//diffuse
	vec3 diffuse = light.diffuse.xyz * diff.xyz * max(0.0f, dot(lightDir, normal)) * attenuation;
	//specular
	vec3 specular = light.specular.xyz * spec.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * attenuation;
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)//maybe also add attenuation calculation to spotlights in order to have flashLights that fade over distance
{
	vec3 lightDir = normalize(light.position.xyz - fragPos);//normalized vector from the fragment's position towards the light
	float angle = dot(-light.spotDirection.xyz , lightDir);//cosine of the angle betwenn the vector from fragment to light and the lights spotDirection vector
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	if (angle > light.cutOff)//cuz the cosine get's bigger when the angle get's smaller(so we actually test if the angle is smaller)
	{
		//diffuse
		diffuse = light.diffuse.xyz * diff.xyz * max(0.0f, dot(lightDir, normal));
		//specular
		specular = light.specular.xyz * spec.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess);
	}
	//ambient-> always add the ambient component
	vec3 ambient = light.ambient.xyz * amb.xyz;

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	vec3 normal = normalize(v_normal);

	for (int i = 0; i < dirLightCount; i++)
	{
		colOutput += calcDirLight(dirLights[i], normal, viewDir);
	}

	for (int i = 0; i < pLightCount; i++)
	{
		colOutput += calcPointLight(pLights[i], normal, v_fragPos, viewDir);
	}

	for (int i = 0; i < sptLightCount; i++)
	{
		colOutput += calcSpotLight(sptLights[i], normal, v_fragPos, viewDir);
	}

	color = vec4(colOutput, 1.0f);
};