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

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_hdr;
uniform sampler2D u_bloom_blur;

void main()
{
	const float gamma = 2.2f;
	vec3 hdrColor = texture(u_hdr, v_texCoord).rgb;
	hdrColor += texture(u_bloom_blur, v_texCoord).rgb;
	//reinhard tone-mapping:
	vec3 mappedColor = hdrColor / (hdrColor + vec3(1.0f));
	color = vec4(pow(mappedColor, vec3(1.0f / gamma)), 1.0f);
};