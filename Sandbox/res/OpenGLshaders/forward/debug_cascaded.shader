#matUniformBlock
#size 0
#endMatUniformBlock

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;


layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;
};

void main()
{
	gl_Position = viewProjMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(0.0f, 1.0f, 0.0f, 0.2f);
};