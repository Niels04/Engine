#matUniformBlock
#size 0
#endMatUniformBlock

#textures
#count 2
#tex 'u_gPosition'
#tex 'u_gNormal'
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

#define radius 0.5f
#define maxKernelSize 64
#define kernelSize 64

layout(location = 0) out float ambientOcclusion;

in vec2 v_texCoord;

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_ssaoNoise;
uniform vec3 u_samples[maxKernelSize];
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

const float noiseSize = textureSize(u_ssaoNoise, 0).x;
const vec2 screenSize = textureSize(u_gPosition, 0);
const vec2 noiseScale = vec2(screenSize.x / noiseSize, screenSize.y / noiseSize);

void main()
{
	vec3 fragPos = (u_viewMat * vec4(texture(u_gPosition, v_texCoord).xyz, 1.0f)).xyz;//transform the position into viewSpace
	vec3 normal = mat3(u_viewMat) * texture(u_gNormal, v_texCoord).xyz;//transform the normal into viewSpace
	if (normal == vec3(0.0f, 0.0f, 0.0f))
		discard;
	vec3 randomRot = texture(u_ssaoNoise, v_texCoord * noiseScale).xyz;//maybe normalize this(already on creation)
	//vec3 randomRot = vec3(0.0f, 0.0f, 1.0f);
	vec3 tangent = normalize(randomRot - normal * dot(randomRot, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);//transforms stuff from tangent-space to view-space
	float occlusion = 0.0f;
	for (int i = 0; i < kernelSize; i++)
	{
		vec3 samplePos = tbn * u_samples[i];
		samplePos = fragPos + samplePos * radius;//to the fragment-position in viewSpace we add a random vector
		vec4 offset = vec4(samplePos, 1.0f);
		offset = u_projMat * offset;//this offset vector is now projected, so that we can use it's xy-coordinates to sample depths around the current fragment
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;//transform to range 0.0f - (-1.0f)
		float sampledDepth = texture(u_gPosition, offset.xy).z;
		//float sampledDepth = -0.5f;
		float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampledDepth));
		occlusion += (sampledDepth >= samplePos.z + 0.025 ? 1.0f : 0.0f) * rangeCheck;//add 1 to the occlusion if the sampled depth is greater than the actual sample's depth -> this is because these z-coordinates are negative
	}
	occlusion = 1.0f - (occlusion / float(kernelSize));
	ambientOcclusion = occlusion;
};