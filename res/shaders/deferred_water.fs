#version 300 es  
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
layout (location = 0) out vec3 DiffuseOut;   
layout (location = 1) out vec3 WorldPosOut;   
layout (location = 2) out vec3 NormalOut;     

uniform mat4 g_MV_matrix;
uniform sampler2D g_diffuse_texture;
uniform sampler2D g_shadow_map;
uniform int g_has_normal_map;
uniform sampler2D g_normal_map;
uniform sampler2D g_mirror_map;
uniform vec3 g_eye_position;
uniform vec3 g_eye_dir;
uniform float g_time;
uniform int g_has_mirror;

varying vec3 v_world_position;
varying vec4 v_light_space_postion;
varying vec2 v_texcoord;
varying vec3 v_normal_line;
varying vec3 v_tangent;

float reflectionFactor = 0.02037;
vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(v_normal_line);
    vec3 Tangent = normalize(v_tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec2 texCoord = v_texcoord;
    texCoord.y += 0.01*g_time;
    texCoord.x += 0.01*g_time;  
    vec3 BumpMapNormal = texture2D(g_normal_map, texCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

void main()
{	
	WorldPosOut     = v_world_position;
    

    vec3 normal;
	if(g_has_normal_map > 0)
	{
		normal = CalcBumpedNormal();
	}else
	{
		normal = normalize(v_normal_line);
	}

    if(g_has_mirror > 0)
    {
                            
        float fastFresnel = max(0.0 ,min(1.0,pow(1.0 - dot(g_eye_dir, normal), 5.0)));
        vec3 waterColor = texture2D(g_diffuse_texture, v_texcoord).xyz;
        vec3 reflectColor = texture2D(g_mirror_map, v_texcoord).xyz;
        DiffuseOut      =  reflectColor*fastFresnel + waterColor;
    }else
    {
        DiffuseOut      = texture2D(g_diffuse_texture, v_texcoord).xyz;
    }
    NormalOut = normal;
	return;
}

