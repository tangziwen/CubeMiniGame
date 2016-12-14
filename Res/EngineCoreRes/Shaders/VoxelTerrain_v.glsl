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
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;
attribute vec3 a_color;
attribute vec3 a_bc;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec3 v_worldNormal;
varying vec3 v_color;
varying vec3 v_bc;
//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = TU_mvpMatrix * vec4(a_position,1.0);
	
	v_position = (TU_mvMatrix * vec4(a_position,1.0)).xyz;
	v_normal = (TU_normalMatrix * vec4(a_normal,0.0)).xyz;
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
	v_worldPos = (TU_mMatrix * vec4(a_position, 1.0)).xyz;
	v_bc = a_bc;
	v_worldNormal = (TU_mMatrix * vec4(a_normal, 0.0)).xyz;
	v_color = a_color;
}