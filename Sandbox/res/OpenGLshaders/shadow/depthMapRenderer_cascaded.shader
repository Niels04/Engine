#matUniformBlock
#size 0
#endMatUniformBlock

#textures
#count 1
#tex 'u_texture'
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
layout (location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2DArray u_texture;

void main()
{
	float depth = texture(u_texture, vec3(v_texCoord, 0.0f)).r;
	color = vec4(vec3(depth), 1.0f);
};