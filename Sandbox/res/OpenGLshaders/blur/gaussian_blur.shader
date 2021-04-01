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

uniform sampler2D u_texture;
uniform bool horizontal;

float weight[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

void main()
{
	vec2 texOffset = 1.0f / textureSize(u_texture, 0);//get the size of a texel in texture-coordinates
	vec3 result = texture(u_texture, v_texCoord).rgb * weight[0];//store the actual texel weighed with the 0th weight
	if (horizontal)
	{
		for (int i = 1; i < 5; i++)
		{
			result += texture(u_texture, v_texCoord + vec2(texOffset.x * i, 0.0f)).rgb * weight[i];
			result += texture(u_texture, v_texCoord - vec2(texOffset.x * i, 0.0f)).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; i++)
		{
			result += texture(u_texture, v_texCoord + vec2(0.0f, texOffset.y * i)).rgb * weight[i];
			result += texture(u_texture, v_texCoord - vec2(0.0f, texOffset.y * i)).rgb * weight[i];
		}
	}
	color = vec4(result, 1.0f);
};