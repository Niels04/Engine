#matUniformBlock
#size 0
#endMatUniformBlock

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;

uniform mat4 u_modelMat;

void main()
{
	gl_Position = u_modelMat * vec4(a_pos, 1.0f);//simply transform into world-space->the geometry-shader does the heavy work
};

#type geometry
#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 u_toLightSpace[6];
uniform uint u_layer;//<-layer of the cubemap's positive x face

out vec4 fragPos;//gets outputed per EmitVertex();

void main()
{
	for (uint face = 0; face < 6; ++face)
	{
		gl_Layer = int(face + u_layer);//specify to which face of the cubemap we render
		for (int i = 0; i < 3; ++i)
		{
			fragPos = gl_in[i].gl_Position;//fragPos remains in world-space to calculate depth, but we still need to transform into the appropriate lightSpace to be able to do projection
			gl_Position = u_toLightSpace[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
};

#type fragment
#version 460 core

#define zFar 60.0f;

in vec4 fragPos;

uniform vec4 u_lightPos;

void main()
{
	float lightDistance = length(u_lightPos.xyz - fragPos.xyz);//get the distance from the light to the fragment
	lightDistance = lightDistance / zFar;//scale to a value between 0 and 1
	gl_FragDepth = lightDistance;//write depth
};