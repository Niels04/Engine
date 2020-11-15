#shader vertex
#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

layout(std140) uniform ViewProjection
{
	mat4 viewMat;
	mat4 projMat;
};

uniform mat4 u_modelMat;

void main()
{
	gl_Position = projMat * viewMat * u_modelMat * vec4(pos, 1.0f); 
};

#shader fragment
#version 460 core

layout (location = 0) out vec4 color;

uniform vec4 u_color;

void main()
{
	color = u_color;
};