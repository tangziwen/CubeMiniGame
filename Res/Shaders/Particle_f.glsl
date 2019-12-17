#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D DiffuseMap;
uniform vec4 TU_color;
uniform float TU_roughness;
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec4 v_color;
uniform vec3 TU_camPos;
//! [0]
void main()
{
	vec4 col = texture2D(DiffuseMap,v_texcoord);
	vec4 albedo = col * TU_color;
	albedo *= v_color;
    // Set fragment color from texture
	gl_FragData[0] = albedo;
}
//! [0]