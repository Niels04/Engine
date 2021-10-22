#matUniformBlock
#size 96
#var vec4f 'alb'
#var float 'ambCoefficient'
#var float 'specCoefficient'
#var float 'shininess'
#var float 'emissiveMultiplier'
#var float 'reflectiveMultiplier'
#endMatUniformBlock

#textures
#count 1
#tex 'u_normalMap'
#endTextures

#type vertex
#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out smooth float v_fragZView;
out vec2 v_texCoord;
out mat3 v_tbn;

layout(std140) uniform ViewProjection
{
	mat4 viewProjMat;
	vec4 padd;
	mat4 viewMat;//needed to transform the normal into viewSpace -> maybe it is possible to just multiply the tbn matrix with this to achieve the transformation???
};

uniform mat4 u_modelMat;
uniform mat3 u_normalMat;

void main()
{
	v_fragZView = (viewMat * u_modelMat * vec4(a_pos, 1.0f)).z;
	v_texCoord = a_texCoord;
	v_tbn = mat3(viewMat) * mat3(normalize(u_normalMat * a_tangent), normalize(u_normalMat * a_bitangent), normalize(u_normalMat * a_normal));//maybe we can get rid of this normalization, because we're using the normalMatrix
	//multiplying the tbn with the viewMat, because we want the normal to be in viewSpace
	gl_Position = viewProjMat * u_modelMat * vec4(a_pos, 1.0f);
};

#type fragment
#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbSpec;

in smooth float v_fragZView;
in vec2 v_texCoord;
in mat3 v_tbn;

layout(std140) uniform material
{
	vec4 alb;
	float ambCoefficient;
	float specCoefficient;
	float shininess;
	float emissiveMultiplier;
	float reflectiveMultiplier;
};

uniform sampler2D u_texture;//albedo texture
uniform sampler2D u_normalMap;

void main()
{
	if (alb.a == 0.0f)
		discard;
	gAlbSpec = alb;
	gAlbSpec.a = specCoefficient;
	vec3 normal = texture(u_normalMap, v_texCoord).rgb;
	normal = normal * 2.0f - 1.0f;
	gNormal = vec4(normalize(v_tbn * normal).xy, emissiveMultiplier, shininess * (normal.z != 0.0f ? sign(normal.z) : 1.0f));//multiply with the tangent-space matrix //<-pass in shininess as the w-component of the normal-texture
	//only store the fragment's viewSpace z for later position reconstruction
	gPosition = vec4(reflectiveMultiplier, 0.0f, v_fragZView, ambCoefficient);//save ambient coefficient as w-component of position-tex
};