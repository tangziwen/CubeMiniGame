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
uniform sampler2D g_depth_map;
uniform vec3 g_eye_position;
uniform mat4 g_light_vp_matrix[4];

uniform sampler2D g_shadow_map[4];

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / g_screen_size;
}

float LinearDepth(float depth,float zNear,float zFar){
    return (depth - zNear)/(zFar - zNear);
}

float CalcShadowFactor(vec3 worldPosition,vec3 normal,vec3 lightDirection)                                                  
{
    int shadowMapLayerIndex = 3;
    vec2 v = CalcTexCoord(); 
    float far = 100.0;
    float currentPixelZ = texture2D(g_depth_map,v).r;
    currentPixelZ = LinearDepth(currentPixelZ ,0.01,100.0);
    if(currentPixelZ*1.0 <=0.2)
    {
        shadowMapLayerIndex = 0;
    }
    else if(currentPixelZ*1.0 <=0.4)
    {
        shadowMapLayerIndex = 1;
    }
    else if(currentPixelZ*1.0 <= 0.7)
    {
        shadowMapLayerIndex = 2;
    }

    vec4 LightSpacePos = g_light_vp_matrix[shadowMapLayerIndex] *vec4(worldPosition,1.0f);
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x = 0.5*ProjCoords.x +0.5;                                                  
    UVCoords.y = 0.5*ProjCoords.y+0.5;                                                  
    float z =  0.5*ProjCoords.z+0.5;

	float xOffset = 1.0f/1024.f;
    float yOffset = 1.0f/768.f;

    float Factor = 0.0f;

    //out of shadow map
    if(UVCoords.x >1.0 || UVCoords.x <0.0 || UVCoords.y >1.0 || UVCoords.y <0.0)
    {
        return 1.0;
    }

    float cosTheta = clamp(dot(normal,lightDirection),0.0,1.0);
    float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0.0,0.01);
    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 Offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec2 UVC = vec2(UVCoords + Offsets);
            float  result = texture2D(g_shadow_map[shadowMapLayerIndex], UVC).x;
			if(result < z - bias)
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



vec4 calculateAmbient(Ambient light , vec3 normal)
{
    if(length(normal) <= 0.5)
    {   
        return vec4(1.0,1.0,1.0,1.0);
    }else
    {
        return vec4(light.color,1.0) * light.intensity;
    }
}

vec4 calculateDiffuse(vec3 normal_line , vec3 light_direction , vec3 color , float intensity,vec3 world_pos)
{
	float shadow_factor =CalcShadowFactor(world_pos,normal_line,light_direction);
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
	return shadow_factor*(diffuse_color+ specular_color);
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
    if(length(normal) != 0.0)
    {
        normal = normalize(normal); 
    }
	vec4 totalLight = calculateAmbient(ambient,normal);
	totalLight += caclculateDirectionLight(normal,directionLight,world_pos);
	gl_FragColor = vec4(color, 1.0) * totalLight;

/*
    float Depth = texture2D(g_depth_map, tex_coord).x;
    Depth = 1.0 - (1.0 - Depth) * 25.0;
    gl_FragColor = vec4(Depth);
*/
	return;
}

