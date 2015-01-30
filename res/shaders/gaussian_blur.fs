#version 300 es  
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform vec2 g_screen_size;
uniform sampler2D g_position_map;
uniform sampler2D g_color_map;
uniform sampler2D g_normal_map;
uniform vec3 g_eye_position;
uniform float g_blur_size;
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / g_screen_size;
}

void main()
{	
	vec2 tex_coord = CalcTexCoord();
	vec3 world_pos = texture2D(g_position_map, tex_coord).xyz;
	vec3 color = texture2D(g_color_map, tex_coord).xyz;
	vec3 normal = texture2D(g_normal_map, tex_coord).xyz;
	normal = normalize(normal);


#define SAMPLER_NUMBER 13
   vec2 arySamplerDistribution[SAMPLER_NUMBER] = vec2[SAMPLER_NUMBER]
   (
        vec2(-6.0f, 0.0f),vec2(-5.0f, 0.0f),vec2(-4.0f, 0.0f),vec2(-3.0f, 0.0f),vec2(-2.0f, 0.0f), vec2(-1.0f, 0.0f), 
    vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f), vec2(2.0f, 0.0f),vec2(3.0f, 0.0f),vec2(4.0f, 0.0f),vec2(5.0f, 0.0f),vec2(6.0f, 0.0f)
      
   );
   float arySamplerWeight[SAMPLER_NUMBER] = float[SAMPLER_NUMBER]
   (   
    0.002216f, 0.008764f, 0.026995f, 0.064759f, 0.120985f, 0.176033f,
      0.199471f,
      0.176033f, 0.120985f, 0.064759f, 0.026995f, 0.008764f, 0.002216f
   );
   vec4 vec4Result = vec4(0.0f);
   for (int i = 0; i < SAMPLER_NUMBER; ++i)
   {
      vec4Result += texture2D(g_color_map, tex_coord + arySamplerDistribution[i]/g_screen_size*g_blur_size)*arySamplerWeight[i];
   }
	gl_FragColor = vec4Result;
	return;
}

