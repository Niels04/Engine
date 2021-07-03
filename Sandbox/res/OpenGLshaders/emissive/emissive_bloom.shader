#matUniformBlock
#size 32
#var vec4f 'emissiveColor'
#var float 'multiplier'
#endMatUniformBlock

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;
};

uniform mat4 u_modelMat;

void main()
{
	gl_Position = viewProjMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

layout(std140) uniform material
{
	vec4 emissiveColor;
	float multiplier;
};

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;//write a bright color in the second attached color attachment

void main()
{
	color = vec4(emissiveColor.xyz * multiplier, 1.0f);
	//calculate the luminance and if it is above a certain threshold output to the second color attachment
	//float luminance = dot(color.xyz, vec3(0.2126f, 0.7152f, 0.0722f));
	//if (luminance > 2.0f)
		brightColor = color;
	//else
		//brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
};