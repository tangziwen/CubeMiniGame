#version 130

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
uniform mat4 TU_mvMatrix;
uniform mat4 TU_mMatrix;
uniform mat4 TU_normalMatrix;
uniform float TU_roughness;
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texcoord;
in vec3 a_color;
in vec3 a_bc;
in vec3 a_mat;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;
out vec3 v_worldPos;
out vec3 v_color;
out vec3 v_bc;
out vec3 v_heightColor;
out vec3 v_mat;

//! [0]
void main()
{

	v_position = (TU_mMatrix * vec4(a_position,1.0)).xyz;
	v_normal = (TU_normalMatrix * vec4(a_normal,0.0)).xyz;
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
	v_worldPos = (TU_mMatrix * vec4(a_position, 1.0)).xyz;
	v_bc = a_bc;
	v_color = a_color;
	v_mat = a_mat;

    // Calculate vertex position in screen space
    gl_Position = TU_mvpMatrix * vec4(a_position,1.0);
	

}