#shader vertex
#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

uniform mat4 u_viewProjMat;

out vec4 v_color;

void main()
{
	v_color = color;
	gl_Position = u_viewProjMat * vec4(pos, 1.0f); 
};

#shader fragment
#version 460 core

layout (location = 0) out vec4 color;

in vec4 v_color;

void main()
{
	color = v_color;
};