#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D DiffuseMap;//albedo

uniform vec4 TU_color;
uniform float TU_roughness;
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec4  v_color;
varying vec3 v_worldPos;
//! [0]
void main()
{
    // Set fragment color from texture
	gl_FragData[0] = pow(texture2D(DiffuseMap,v_texcoord), vec4(2.2))*TU_color * v_color;
	gl_FragData[1] = vec4(v_position,1.0);
	gl_FragData[2] = vec4(normalize(v_normal),1.0);
	gl_FragData[3] = vec4(TU_roughness,0.0,0.0,1.0);
}
//! [0]