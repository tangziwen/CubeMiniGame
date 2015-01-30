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
uniform float g_blur_samples;
uniform float g_blur_radius;
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

    if (g_blur_radius > 0.0 && g_blur_samples > 1.0)
    {
        vec3 col = vec3(0);
        vec2 unit = 1.0 / g_screen_size.xy;
        
        float r = g_blur_radius;
        float sampleStep = r / g_blur_samples;
        
        float count = 0.0;
        
        for(float x = -r; x < r; x += sampleStep)
        {
            for(float y = -r; y < r; y += sampleStep)
            {
                float weight = (r - abs(x)) * (r - abs(y));
                col += texture2D(g_color_map,tex_coord + vec2(x * unit.x, y * unit.y)).rgb* weight;
                count += weight;
            }
        }
        gl_FragColor = vec4(col / count,1.0);
    }else
    {
        gl_FragColor = vec4(color,1.0);
    }
	return;
}

