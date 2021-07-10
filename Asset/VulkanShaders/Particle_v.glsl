#extension GL_ARB_separate_shader_objects : enable


layout(set = 0, binding = 0) uniform UniformBufferObjectMat {
    vec4 TU_color;
} t_shaderUnifom;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	mat4 mvp;
	mat4 wv;
	mat4 TU_mMatrix;
	mat4 view;
	mat4 projection;
} t_ObjectUniform;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 7) in mat4 a_instance_offset;
layout(location = 11) in vec4 a_instance_offset2;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_tangent;
layout(location = 4) out vec4 v_color;

//! [0]
void main()
{

    // Calculate vertex position in screen space
    
	mat4 modelMatrix = t_ObjectUniform.TU_mMatrix * a_instance_offset;

    modelMatrix[3][0] = a_instance_offset[3][0];
    modelMatrix[3][1] = a_instance_offset[3][1];
    modelMatrix[3][2] = a_instance_offset[3][2];

    mat4 modelView = t_ObjectUniform.view * modelMatrix;
    // // First colunm.
    modelView[0][0] = length( vec3(a_instance_offset[0][0], a_instance_offset[0][1], a_instance_offset[0][2])) ;
    modelView[0][1] = 0.0;
    modelView[0][2] = 0.0;

    // // if (spherical == 1)
    // // {
    // // Second colunm.
    modelView[1][0] = 0.0;
    modelView[1][1] = length( vec3(a_instance_offset[1][0], a_instance_offset[1][1], a_instance_offset[1][2]));//a_instance_offset[1][1];
    modelView[1][2] = 0.0;
    // // }

    // // Thrid colunm.
    modelView[2][0] = 0.0;
    modelView[2][1] = 0.0;
    modelView[2][2] = length( vec3(a_instance_offset[2][0], a_instance_offset[2][1], a_instance_offset[2][2]));//a_instance_offset[2][2];

    gl_Position = t_ObjectUniform.projection * modelView * vec4(inPosition,1.0);
    //gl_Position.z -= TU_depthBias / (TU_depthBias + gl_Position.w);
	v_normal = (t_ObjectUniform.TU_mMatrix * modelMatrix* vec4(inNormal,0.0)).xyz;
	v_tangent = (t_ObjectUniform.TU_mMatrix * modelMatrix * vec4(inTangent,0.0)).xyz;
	v_color = a_instance_offset2;
	v_texcoord = texcoord;
	fragColor = inColor;
}