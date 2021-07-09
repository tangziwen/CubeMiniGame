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

#ifdef FLAG_EnableInstanced
layout(location = 7) in mat4 a_instance_offset;
layout(location = 11) in vec4 a_instance_offset2;
#endif

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_tangent;
layout(location = 4) out vec3 v_color;
void main() {
	
	
    mat4 modelView = t_ObjectUniform.wv;
    vec3 up = modelView[1].xyz;
    vec3 front = vec3(0, 0, 1);
    if(dot(normalize(up), normalize(front)) > 0.95){
		vec3 right = vec3(0, 1, 0);
		front = normalize(cross(right, up));
		right = normalize(cross(front, up));
		modelView[0][0] = right.x;
		modelView[0][1] = right.y;
		modelView[0][2] = right.z;

		modelView[2][0] = -front.x;
		modelView[2][1] = -front.y;
		modelView[2][2] = -front.z;
    }
	else
	{
		vec3 right = normalize(cross(front, up));
		front = normalize(cross(right, up));
		modelView[0][0] = right.x;
		modelView[0][1] = right.y;
		modelView[0][2] = right.z;

		modelView[2][0] = -front.x;
		modelView[2][1] = -front.y;
		modelView[2][2] = -front.z;
	}
    mat4 mvp = t_ObjectUniform.projection * modelView;
    
    // Calculate vertex position in screen space
    gl_Position = mvp * vec4(inPosition,1.0);
	
	
	v_texcoord = texcoord;
	fragColor = inColor;
}