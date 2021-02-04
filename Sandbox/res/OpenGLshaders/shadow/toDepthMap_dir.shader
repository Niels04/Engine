#matUniformBlock
#size 0
#endMatUniformBlock

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;

uniform mat4 u_toLightSpace;
uniform mat4 u_modelMat;

void main()
{
	gl_Position = u_toLightSpace * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

void main()
{
	//gl_FragDepth = gl_FragCoord.z;
};