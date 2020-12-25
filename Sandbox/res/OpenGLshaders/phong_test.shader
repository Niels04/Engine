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

layout(std140) uniform material
{
	vec4 amb;
	vec4 diff;
	vec4 spec;
	float shininess;
};

layout(std140) uniform testBlock
{
	int count;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_viewPos;
in vec3 v_fragPos;

void main()
{
	vec3 colOutput = vec3(0.0f);
	
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	vec3 normal = normalize(v_normal);

	colOutput = direction.xyz;

	color = vec4(colOutput, 1.0f);
};