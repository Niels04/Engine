#matUniformBlock
#size 0
#endMatUniformBlock

#textures
#count 1
#tex 'u_skybox'
#endTextures

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;

out vec3 texCoords;

/*layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;
};*/

uniform mat4 u_viewProj;

void main()
{
	texCoords = a_pos;
	vec4 pos = u_viewProj * vec4(a_pos, 1.0f);
	gl_Position = pos.xyww;//assign the w-component to the z-component, so that we get a value of 1.0f in the depthBuffer
};

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 texCoords;

uniform samplerCube u_skybox;

void main()
{
	vec4 col_rgbe = texture(u_skybox, texCoords).rgba * 255;//multiply to the range 0 - 255 instead of 0 to 1
	//convert from rgbe to hdr rgba
	float factor = pow(2.0f, col_rgbe.a - 128.0f) / 256.0f;
	color = vec4(col_rgbe.rgb * factor, 1.0f);
};