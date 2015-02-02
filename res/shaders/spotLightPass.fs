#version 300 es  
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


struct SpotLight
{
	vec3 direction;
	float range;
	vec3 pos;
	vec3 color;
	float intensity;
	float ang;
	float outterAng;
};

uniform SpotLight spotLight[1];

uniform vec2 g_screen_size;
uniform sampler2D g_position_map;
uniform sampler2D g_color_map;
uniform sampler2D g_normal_map;
uniform sampler2D g_shadow_map;
uniform vec3 g_eye_position;

uniform mat4 g_light_vp_matrix;

float CalcShadowFactor(sampler2D the_shadow_map, vec4 LightSpacePos)                                                  
{                       	
	float bias = 0.005f;
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x = 0.5*ProjCoords.x +0.5;                                                  
    UVCoords.y = 0.5*ProjCoords.y+0.5;                                                  
    float z =  0.5*ProjCoords.z+0.5;

	float xOffset = 1.0f/1024.f;
    float yOffset = 1.0f/768.f;

    float Factor = 0.0f;

    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 Offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec2 UVC = vec2(UVCoords + Offsets);
            float  result = texture2D(the_shadow_map, UVC).x;
			if(result < z -0.00001f)
			{
			Factor +=0.0f;
			}
			else{
			Factor +=1.0f;	
			}
        }
    }                                                                    
	return Factor/9.0f;
}

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / g_screen_size;
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
	return diffuse_color+ specular_color;
}

vec4 calculateSpotLight(vec3 normal_line,SpotLight light,vec3 world_pos)
{
	vec4 light_space_pos = g_light_vp_matrix *vec4(world_pos,1.0f);
	float shadow_factor =CalcShadowFactor(g_shadow_map,light_space_pos);
	vec3 pixelToSourceDirection = world_pos - light.pos;
	float distance = length(pixelToSourceDirection);
	pixelToSourceDirection = normalize(pixelToSourceDirection);
	float current_ang = dot(normalize(pixelToSourceDirection), normalize(light.direction));
	if(current_ang > light.ang)
	{
		vec4 pointColor = calculateDiffuse(normal_line,light.direction, light.color , light.intensity,world_pos);
		float attenuation = clamp (1.0f - (distance/ light.range ),0.0f,1.0f);
		pointColor *=  attenuation;
		return shadow_factor*pointColor;
	}else if(current_ang > light.outterAng){
		vec4 pointColor = calculateDiffuse(normal_line,light.direction, light.color , light.intensity,world_pos);
		float attenuation = clamp (1.0f - (distance/ light.range ),0.0f,1.0f);
		attenuation *= smoothstep(  light.outterAng , light.ang , current_ang);
		pointColor *=  attenuation;
		return shadow_factor*pointColor;
	}
	else {
			return vec4(0.0,0,0,1.0);
	}
}

void main()
{	
	vec2 tex_coord = CalcTexCoord();
	vec3 world_pos = texture2D(g_position_map, tex_coord).xyz;
	vec3 color = texture2D(g_color_map, tex_coord).xyz;
	vec3 normal = texture2D(g_normal_map, tex_coord).xyz;
	normal = normalize(normal);
	vec4 spot_light_color = calculateSpotLight(normal,spotLight[0],world_pos);
	gl_FragColor = vec4(color, 1.0f) * spot_light_color;
	return;
}

