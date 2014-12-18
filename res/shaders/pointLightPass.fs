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

struct PointLight
{
	float range;
	vec3 pos;
	vec3 color;
	float intensity;
};

uniform PointLight pointLight[1];
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
            specular_color = vec4(color, 1.0f) * 1.0f * SpecularFactor;
        }
    }
    else {
        diffuse_color = vec4(0, 0, 0, 1.0);
    }
	return diffuse_color + specular_color;
}

vec4 calculatePointLight(vec3 normal_line,PointLight light,vec3 world_pos)
{
	vec3 pointLightDirection = world_pos - light.pos;
	float distance = length(pointLightDirection);
	pointLightDirection = normalize(pointLightDirection);
	vec4 pointColor = calculateDiffuse(normal_line,pointLightDirection, light.color , light.intensity,world_pos);
	float attenuation = clamp (1.0f - (distance/ light.range ),0.0f,1.0f);
	pointColor *=  attenuation;
	return pointColor;
}

void main()
{	
	vec2 tex_coord = CalcTexCoord();
	vec3 world_pos = texture2D(g_position_map, tex_coord).xyz;
	vec3 color = texture2D(g_color_map, tex_coord).xyz;
	vec3 normal = texture2D(g_normal_map, tex_coord).xyz;
	normal = normalize(normal);
	vec4 point_light_color = calculatePointLight(normal,pointLight[0],world_pos);
	gl_FragColor = vec4(color, 1.0) * point_light_color;
	return;
}

