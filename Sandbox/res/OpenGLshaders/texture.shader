#matUniformBlock
#size 0
#endMatUniformBlock

#textures
#count 1
#tex 'u_texture'
#endTextures

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;

layout(std140) uniform ViewProjection
{
	mat4 viewMat;
	mat4 projMat;
};

uniform mat4 u_modelMat;

out vec2 v_texCoord;

void main()
{
	v_texCoord = a_texCoord;
	gl_Position = projMat * viewMat * u_modelMat * vec4(a_pos, 1.0f); 
};

#type fragment
#version 460 core

layout (location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
	color = texture(u_texture, v_texCoord);//texture is a function, that samples the texture at slot(first argument) with the texture-coordinates(second argument)
};