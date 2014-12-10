#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
const int MAX_POINT_LIGHTS = 2;                                                     
const int MAX_SPOT_LIGHTS = 2;    

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

struct PointLight
{
	float range;
	vec3 pos;
	vec3 color;
	float intensity;
};


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

uniform int g_point_light_amount;
uniform int g_spot_light_amount;
uniform Ambient ambient;
uniform DirectionalLight directionLight;

uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

uniform mat4 g_MV_matrix;
uniform sampler2D g_diffuse_texture;
uniform sampler2D g_shadow_map;
uniform int g_has_normal_map;
uniform sampler2D g_normal_map;
uniform vec3 g_eye_position;

varying vec3 v_world_position;
varying vec4 v_light_space_postion;
varying vec2 v_texcoord;
varying vec3 v_normal_line;
varying vec3 v_tangent;


vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(v_normal_line);
    vec3 Tangent = normalize(v_tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture2D(g_diffuse_texture, v_texcoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

float CalcShadowFactor(sampler2D the_shadow_map, vec4 LightSpacePos)                                                  
{                       	
	float bias = 0.005;
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x = 0.5*ProjCoords.x +0.5;                                                  
    UVCoords.y = 0.5*ProjCoords.y+0.5;                                                  
    float z =  0.5*ProjCoords.z+0.5;

	float xOffset = 1.0/2000;
    float yOffset = 1.0/2000;

    float Factor = 0.0;

    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 Offsets = vec2(x * xOffset, y * yOffset);
            vec2 UVC = vec2(UVCoords + Offsets);
            float  result = texture2D(the_shadow_map, UVC).x;
			if(result < z -0.00001)
			{
			Factor +=0;
			}
			else{
			Factor +=1;	
			}
        }
    }                                                                    
	return Factor/9;
}

vec4 calculateAmbient(Ambient light)
{
	return vec4(light.color,1.0) * light.intensity;
}
vec4 calculateDiffuse(vec3 normal_line , vec3 light_direction , vec3 color , float intensity)
{
	float diffuse_factor = dot(normalize(normal_line), normalize(-light_direction));
	
	vec4 diffuse_color;
	vec4 specular_color;
    if (diffuse_factor > 0) {
        diffuse_color = vec4(color,1.0) * diffuse_factor * intensity;

        vec3 VertexToEye = normalize(g_eye_position - v_world_position);
        vec3 LightReflect = normalize(reflect(light_direction, normal_line));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, 32);
        if (SpecularFactor > 0) {
            specular_color = vec4(color, 1.0f) * 1 * SpecularFactor;
        }
    }
    else {
        diffuse_color = vec4(0, 0, 0, 1.0);
    }
	return diffuse_color+ specular_color;
}


vec4 caclculateDirectionLight(vec3 normal_line,DirectionalLight light)
{
	return calculateDiffuse(normal_line , light.direction , light.color , light.intensity);
}




vec4 calculatePointLight(vec3 normal_line,PointLight light)
{
	vec3 pointLightDirection = v_world_position - light.pos;
	float distance = length(pointLightDirection);
	pointLightDirection = normalize(pointLightDirection);
	vec4 pointColor = calculateDiffuse(normal_line,pointLightDirection, light.color , light.intensity);
	float attenuation = clamp (1 - (distance/ light.range ),0,1);
	pointColor *=  attenuation;
	return pointColor;
}


vec4 calculateSpotLight(vec3 normal_line,SpotLight light)
{
	float shadow_factor =CalcShadowFactor(g_shadow_map,v_light_space_postion);
	vec3 pixelToSourceDirection = v_world_position - light.pos;
	float distance = length(pixelToSourceDirection);
	pixelToSourceDirection = normalize(pixelToSourceDirection);
	float current_ang = dot(normalize(pixelToSourceDirection), normalize(light.direction));
	if(current_ang > light.ang)
	{
		vec4 pointColor = calculateDiffuse(normal_line,light.direction, light.color , light.intensity);
		float attenuation = clamp (1 - (distance/ light.range ),0,1);
		pointColor *=  attenuation;
		return shadow_factor*pointColor;
	}else if(current_ang > light.outterAng){
		vec4 pointColor = calculateDiffuse(normal_line,light.direction, light.color , light.intensity);
		float attenuation = clamp (1 - (distance/ light.range ),0,1);
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
	vec3 normal;
	if(g_has_normal_map >0 )
	{
		normal = CalcBumpedNormal();
	}else
	{
		normal = v_normal_line;
	}

	vec4 totalLight = vec4(0,0,0,1.0);
	totalLight += calculateAmbient(ambient);
	totalLight += caclculateDirectionLight(normal,directionLight);
	
	for(int j = 0;j<g_spot_light_amount;j++)
	{
	totalLight += calculateSpotLight(normal,spotLight[j]);
	}
	for(int i = 0;i<g_point_light_amount;i++)
	{
	totalLight += calculatePointLight(normal,pointLight[i]);
	}
	
	gl_FragColor = texture2D(g_diffuse_texture,v_texcoord)*totalLight;
	return;
}

