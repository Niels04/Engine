#matUniformBlock
#size 96
#var vec4f 'alb'
#var float 'ambCoefficient'
#var float 'specCoefficient'
#var float 'shininess'
#var float 'emissiveMultiplier'
#var float 'reflectiveMultiplier'
#endMatUniformBlock

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out smooth float v_fragZView;//the fragments z-component in viewSpace
out vec3 v_normal;

layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;
	vec4 padd;
	mat4 viewMat;//in order to transform the normal into viewSpace
};

uniform mat4 u_modelMat;
uniform mat3 u_normalMat;

void main()
{
	v_fragZView = (viewMat * u_modelMat * vec4(a_pos, 1.0f)).z;//store the fragment's z-component in viewSpace
	v_normal = mat3(viewMat) * u_normalMat * a_normal;
	gl_Position = viewProjMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbSpec;

in smooth float v_fragZView;//the fragments z-component in viewSpace
in vec2 v_texCoord;
in vec3 v_normal;

layout(std140) uniform material
{
	vec4 alb;
	float ambCoefficient;
	float specCoefficient;
	float shininess;
	float emissiveMultiplier;
	float reflectiveMultiplier;
};

void main()
{
	if (alb.a == 0.0f)
		discard;
	vec3 normal = normalize(v_normal);
	//only store the fragment's viewSpace z for later position reconstruction
	gPosition = vec4(reflectiveMultiplier, 0.0f, v_fragZView, ambCoefficient);//<-pass the ambient coefficient as the w-compontent of the position-texture
	gNormal = vec4(normal.xy, emissiveMultiplier, shininess * (normal.z != 0.0f ? sign(normal.z) : 1.0f) );//multiply with the tangent-space matrix //<-pass in shininess as the w-component of the normal-texture
	gAlbSpec.xyz = alb.xyz;
	gAlbSpec.a = specCoefficient;
};