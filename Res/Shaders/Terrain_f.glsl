#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D TU_tex1;
uniform sampler2D TU_rockNormal;
uniform sampler2D TU_grass;

uniform vec4 TU_color;
uniform float TU_roughness;
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec3 v_color;
//! [0]
void main()
{
    // Set fragment color from texture
	float rock_factor = 15.0;
	vec3 base_normal = v_normal;
	vec3 tangent = normalize(cross(base_normal, vec3(0.0, 0.0, 1.0)));
    vec3 bitangent = normalize(cross(tangent, base_normal));
	mat3 orthobasis = mat3(tangent, base_normal, bitangent);
	
	
	vec3 rock_color = texture2D(TU_tex1, v_texcoord*rock_factor).rgb * 0.8;
	vec3 rockNormal = orthobasis * normalize((texture2D(TU_rockNormal, v_texcoord*rock_factor).xyz-0.5));
	
	float grass_factor = 2.0;
    vec3 grass_color = texture2D(TU_grass, v_texcoord*grass_factor).rgb * 0.8;

	vec3 dirt = vec3(85.0/255.0, 34.0/255.0, 0.0);
    vec3 soil = grass_color;
	
	float mix_factor = clamp(v_worldPos.y * 0.5, 0.0, 1.0);
	vec3 color = mix(rock_color, soil, 1.0 - mix_factor).xyz * v_color;
	vec3 normal = mix(rockNormal, base_normal, 1.0 - mix_factor).xyz;
    //vec3 rock_color = pow(texture2D(rock, uv*rock_factor).rgb, vec3(0.5)) * 1.5;
	
    gl_FragData[0] = vec4(color.rgb, 1.0);
	gl_FragData[1] = vec4(v_worldPos,1.0);
	gl_FragData[2] = vec4(normalize(rockNormal),1.0);		
	gl_FragData[3] = vec4(TU_roughness,0.0,0.0,1.0);
}
//! [0]