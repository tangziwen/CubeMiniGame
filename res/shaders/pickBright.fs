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
#define BRIGHT_THRESHOLD 0.99
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / g_screen_size;
}

bool isBright(vec3 color)
{
    float grayFactor = 0.3*color.r + 0.6*color.g + 0.1*color.b;
    if(grayFactor > BRIGHT_THRESHOLD)
    {
        return true;
    }else{
        return false;
    }
}

void main()
{	
	vec2 tex_coord = CalcTexCoord();
	vec3 world_pos = texture2D(g_position_map, tex_coord).xyz;
	vec3 color = texture2D(g_color_map, tex_coord).xyz;
	vec3 normal = texture2D(g_normal_map, tex_coord).xyz;
	normal = normalize(normal);
	if(isBright(color))
	{
		gl_FragColor = vec4(color,1.0f);
	}else{
		gl_FragColor = vec4(0.0f,0.0f,0.0f,1.0f);
	}
	return;
}