#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mvp;
	mat4 wv;
	mat4 TU_mMatrix;
	mat4 view;
	mat4 projection;
} t_ObjectUniform;

layout(set = 1, binding = 0) uniform UniformBufferObjectMat {
    vec4 TU_color;
} t_shaderUnifom;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 a_matBlend;
layout(location = 6) in ivec3 a_mat;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_tangent;
layout(location = 4) out vec3 v_matBlend;
layout(location = 5) out float[16] v_mat;
layout(location = 22) out vec3 v_worldPos;

void main() {
    gl_Position = t_ObjectUniform.mvp * vec4(inPosition, 1.0);
	
	v_texcoord = texcoord;
	
	v_worldPos = (t_ObjectUniform.TU_mMatrix * vec4(inPosition, 1.0)).xyz;
	fragColor = inColor * t_shaderUnifom.TU_color;
	v_normal = (t_ObjectUniform.TU_mMatrix * vec4(inNormal,0.0)).xyz;
	v_tangent = (t_ObjectUniform.TU_mMatrix * vec4(inTangent,0.0)).xyz;
	v_mat = float[16](0.0,0.0,0.0,0.0, 0.0,0.0,0.0,0.0, 0.0,0.0,0.0,0.0, 0.0,0.0,0.0,0.0);
	v_mat[a_mat.x] = a_matBlend.x;
	if (a_mat.y != a_mat.x)
	{
		v_mat[a_mat.y] = a_matBlend.y;
	}
	if (a_mat.z != a_mat.x && a_mat.z != a_mat.y)
	{
		v_mat[a_mat.z] = a_matBlend.z;
	}
}