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

/*//note that this is a kernelSize of 9
float offset[5] = float[](0.0f, 1.0f, 2.0f, 3.0f, 4.0f);
float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
//now the whole thing with linear filtering:
float weight_linear[3] = float[](0.2270270270, weight[1] + weight[2], weight[3] + weight[4]);
float offset_linear[3] = float[](0.0f, (offset[1] * weight[1] + offset[2] * weight[2]) / weight_linear[1], (offset[3] * weight[3] + offset[4] * weight[4]) / weight_linear[2]);*/
//now a kernelSize of 29
float offset[15] = float[](0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f);
float weight[15] = float[](0.0035108587105570268, 0.0034752157826179668, 0.0033704434657192616, 0.0032027951718746569, 0.0029820033755482418, 0.0027203447364603666, 0.0024315130445504121,
	0.0021294434498719990, 0.0018272268407959907, 0.0015362279707875905, 0.0012654812582996776, 0.0010213923820763810, 0.00080773026982803209, 0.00062585964550472550, 0.00047514305799696408);
//now linear filtering:
float weight_linear[8] = float[](0.0035108587105570268, weight[1] + weight[2], weight[3] + weight[4], weight[5] + weight[6], weight[7] + weight[8], weight[9] + weight[10], weight[11] + weight[12],
	weight[13] + weight[14]);
float offset_linear[8] = float[](0.0f, (offset[1] * weight[1] + offset[2] * weight[2]) / weight_linear[1], (offset[3] * weight[3] + offset[4] * weight[4]) / weight_linear[2],
	(offset[5] * weight[5] + offset[6] * weight[6]) / weight_linear[3], (offset[7] * weight[7] + offset[8] * weight[8]) / weight_linear[4],
	(offset[9] * weight[9] + offset[10] * weight[10]) / weight_linear[5], (offset[11] * weight[11] + offset[12] * weight[12]) / weight_linear[6],
	(offset[13] * weight[13] + offset[14] * weight[14]) / weight_linear[7]);

void main()
{
	vec2 texOffset = 1.0f / textureSize(u_texture, 0);//get the size of a texel in texture-coordinates
	vec3 result = texture(u_texture, v_texCoord).rgb * weight_linear[0];//store the actual texel weighed with the 0th weight

	for (int i = 1; i < 8; i++)
	{
		result += texture(u_texture, v_texCoord + vec2(texOffset.x * offset_linear[i], 0.0f)).rgb * weight_linear[i];
		result += texture(u_texture, v_texCoord - vec2(texOffset.x * offset_linear[i], 0.0f)).rgb * weight_linear[i];
	}
	color = vec4(10.0f * result, 1.0f);
};