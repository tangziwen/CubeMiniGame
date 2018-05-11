#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
uniform mat4 TU_mvMatrix;
uniform mat4 TU_mMatrix;
uniform mat4 TU_normalMatrix;
uniform mat4 TU_lightWVP;
uniform float TU_roughness;
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;
attribute vec3 a_color;
attribute vec3 a_bc;
attribute vec3 a_mat;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec3 v_worldNormal;
varying vec3 v_color;
varying vec3 v_bc;
varying vec3 v_heightColor;
varying vec3 v_mat;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = TU_lightWVP * vec4(a_position,1.0);
}