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
uniform vec3 TU_camPos;
//! [0]
void main()
{

vec3 worldNormal = v_normal;
#ifdef FLAG_EnableDoubleSide
	if(gl_FrontFacing)
	{
		worldNormal = v_normal;
	}
	else{
		worldNormal = -v_normal;
	}
	worldNormal = mix(worldNormal, vec3(0,1, 0), 0.2);
#endif
	vec4 col = texture2D(DiffuseMap,v_texcoord);
	vec4 albedo = vec4(pow(col.rgb, vec3(2.2)), col.a)*TU_color;
	if(albedo.a <0.4)
	{
		discard;
	}
    // Set fragment color from texture
    gl_FragData[0] = albedo;
	gl_FragData[1] = vec4(v_position, 1.0);
	gl_FragData[2] = vec4(worldNormal, 1.0);
	gl_FragData[3] = vec4(TU_roughness,0.0,0.0,1.0);
}
//! [0]