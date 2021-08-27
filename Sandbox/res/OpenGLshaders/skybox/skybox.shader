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
	color = vec4(texture(u_skybox, texCoords).rgb, 1.0f);
};