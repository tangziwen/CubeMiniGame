#extension GL_ARB_separate_shader_objects : enable
layout(set = 0, binding = 0) uniform UniformBufferObjectMat {
    vec4 TU_color;
} t_shaderUnifom;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} t_ObjectUniform;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;
layout(location = 7) in vec4 inOverLayColor;


layout(location = 8) in mat4 a_instance_offset;
layout(location = 12) in vec4 a_instance_offset2;
layout(location = 13) in vec4 a_instance_offset3;


layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec4 v_fragOverLayColor;
layout(location = 3) out vec4 v_extraInstanceInfo;
void main() {

	mat4 modelMatrix = a_instance_offset;
	gl_Position = t_ObjectUniform.mvp * modelMatrix * vec4(inPosition, 1.0);
	
	v_texcoord = texcoord;
	
	v_fragOverLayColor = a_instance_offset2;
	v_extraInstanceInfo = a_instance_offset3;
}
