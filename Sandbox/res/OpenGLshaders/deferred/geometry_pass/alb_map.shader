#matUniformBlock
#size 48
#var float 'ambCoefficient'
#var float 'specCoefficient'
#var float 'shininess'
#endMatUniformBlock

#textures
#count 1
#tex 'u_texture'
#endTextures

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out vec3 v_fragPos;
out vec2 v_texCoord;
out vec3 v_normal;

layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;
	vec4 viewPos;//not needed in this shader, but in the lighting-shader <- maybe just remove this
};

uniform mat4 u_modelMat;
uniform mat3 u_normalMat;

void main()
{
	v_fragPos = (u_modelMat * vec4(a_pos, 1.0f)).xyz;
	v_texCoord = a_texCoord;
	v_normal = u_normalMat * a_normal;
	gl_Position = viewProjMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbSpec;

in vec3 v_fragPos;
in vec2 v_texCoord;
in vec3 v_normal;

layout(std140) uniform material
{
	float ambCoefficient;
	float specCoefficient;
	float shininess;
};

uniform sampler2D u_texture;//albedo texture

void main()
{
	vec3 normal = normalize(v_normal);
	gPosition = vec4(v_fragPos, ambCoefficient);//save ambient coeffiecient as w-component of position-tex
	gNormal = vec4(normal, shininess);//multiply with the tangent-space matrix //<-pass in shininess as the w-component of the normal-texture
	gAlbSpec = texture(u_texture, v_texCoord);
	if (gAlbSpec.a == 0.0f)
		discard;
	gAlbSpec.a = specCoefficient;
};