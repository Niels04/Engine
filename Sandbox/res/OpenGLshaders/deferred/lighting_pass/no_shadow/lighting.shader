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

#define BG_col vec4(0.0171f, 0.0171f, 0.0171f, 1.0f)

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

struct directionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec4 specular;
	mat4 toLightSpace;
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

vec3 calcDirLight(directionalLight light, vec3 normal, vec3 viewDir, vec3 texColor, float specularIntensity, float shininess, float ambCoefficient)
{
	//light.direction must be normalized!!!
	//ambient
	vec3 ambient = light.ambient * (texColor * ambCoefficient);
	//diffuse
	vec3 diffuse = light.diffuse * texColor * max(0.0f, dot(-light.direction, normal));
	//specular
	vec3 specular = light.specular.xyz * pow(max(0.0f, dot(reflect(light.direction, normal), viewDir)), shininess) * specularIntensity;//when using"reflect()" put in the vector from the lightSource towards the fragment
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor, float specularIntensity, float shininess, float ambCoefficient)
{
	float distance = length(fragPos - light.position.xyz);
	float attenuation = 1.0f / (light.attenuationFactors.x + light.attenuationFactors.y * distance + light.attenuationFactors.z * (distance * distance));//Dämpfung bei weiterer Entfernung
	vec3 lightDir = normalize(light.position.xyz - fragPos);//direction from the fragment to the light
	//ambient
	vec3 ambient = light.ambient.xyz * texColor * attenuation * ambCoefficient;
	//diffuse
	vec3 diffuse = light.diffuse.xyz * texColor * max(0.0f, dot(lightDir, normal)) * attenuation;
	//specular
	vec3 specular = light.specular.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * attenuation * specularIntensity;
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor, float specularIntensity, float shininess, float ambCoefficient)//maybe also add attenuation calculation to spotlights in order to have flashLights that fade over distance
{
	vec3 lightDir = normalize(light.position.xyz - fragPos);//normalized vector from the fragment's position towards the light
	float angle = dot(-light.spotDirection.xyz, lightDir);//cosine of the angle betwenn the vector from fragment to light and the lights spotDirection vector
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	if (angle > light.specular.w)//cuz the cosine get's bigger when the angle get's smaller(so we actually test if the angle is smaller)
	{
		//diffuse
		diffuse = light.diffuse.xyz * texColor * max(0.0f, dot(lightDir, normal));
		//specular
		specular = light.specular.xyz * pow(max(0.0f, dot(reflect(-lightDir, normal), viewDir)), shininess) * specularIntensity;
	}
	//ambient-> always add the ambient component
	vec3 ambient = light.ambient.xyz * texColor * ambCoefficient;
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 colOutput = vec3(0.0f);
	vec4 fragPos = texture(u_gPosition, v_texCoord).rgba;
	float ambCoefficient = fragPos.w;
	vec4 AlbSpec = texture(u_gAlbSpec, v_texCoord).rgba;
	float specIntensity = AlbSpec.a;
	vec4 normal = texture(u_gNormal, v_texCoord).rgba;
	float shininess = normal.w;//<-retrieve the shininess from the normal's w-component
	vec3 viewDir = normalize(viewPos.xyz - fragPos.xyz);

	brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (normal.xyz != vec3(0.0f, 0.0f, 0.0f))
	{
		for (int i = 0; i < dirLightCount; i++)
		{
			colOutput += calcDirLight(dirLights[i], normal.xyz, viewDir, AlbSpec.xyz, specIntensity, shininess, ambCoefficient);
		}

		for (int i = 0; i < pLightCount; i++)
		{
			colOutput += calcPointLight(pLights[i], normal.xyz, fragPos.xyz, viewDir, AlbSpec.xyz, specIntensity, shininess, ambCoefficient);
		}

		for (int i = 0; i < sptLightCount; i++)
		{
			colOutput += calcSpotLight(sptLights[i], normal.xyz, fragPos.xyz, viewDir, AlbSpec.xyz, specIntensity, shininess, ambCoefficient);
		}

		hdrColor = vec4(colOutput, 1.0f);
	}
	else
	{
		hdrColor = BG_col;
	}
};