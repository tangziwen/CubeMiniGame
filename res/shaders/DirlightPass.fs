#version 300 es  
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif



struct Ambient
{
	vec3 color;
	float intensity;
};

struct  DirectionalLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};

uniform DirectionalLight directionLight;
uniform Ambient ambient;

uniform vec2 g_screen_size;
uniform sampler2D g_position_map;
uniform sampler2D g_color_map;
uniform sampler2D g_normal_map;
uniform vec3 g_eye_position;
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / g_screen_size;
}

vec4 calculateAmbient(Ambient light)
{
	return vec4(light.color,1.0) * light.intensity;
}

vec4 calculateDiffuse(vec3 normal_line , vec3 light_direction , vec3 color , float intensity,vec3 world_pos)
{
	float diffuse_factor = dot(normalize(normal_line), normalize(-light_direction));
	vec4 diffuse_color;
	vec4 specular_color;
    if (diffuse_factor > 0.0f ) {
        diffuse_color = vec4(color,1.0f) * diffuse_factor * intensity;

        vec3 VertexToEye = normalize(g_eye_position - world_pos);
        vec3 LightReflect = normalize(reflect(light_direction, normal_line));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, 32.0f);
        if (SpecularFactor > 0.0f) {
            specular_color = vec4(color, 1.0f) * 1.0f * SpecularFactor*intensity;
        }
    }
    else {
        diffuse_color = vec4(0, 0, 0, 1.0);
    }
	return diffuse_color+ specular_color;
}

vec4 caclculateDirectionLight(vec3 normal_line,DirectionalLight light,vec3 world_pos)
{
	return calculateDiffuse(normal_line , light.direction , light.color , light.intensity,world_pos);
}

void main()
{	
	vec2 tex_coord = CalcTexCoord();
	vec3 world_pos = texture2D(g_position_map, tex_coord).xyz;
	vec3 color = texture2D(g_color_map, tex_coord).xyz;
	vec3 normal = texture2D(g_normal_map, tex_coord).xyz;
	normal = normalize(normal);
	vec4 totalLight = calculateAmbient(ambient);
	totalLight += caclculateDirectionLight(normal,directionLight,world_pos);
	gl_FragColor = vec4(color, 1.0) * totalLight;
	return;
}

