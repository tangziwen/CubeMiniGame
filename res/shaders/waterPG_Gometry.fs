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
uniform sampler2D g_refract_map;
uniform vec3 g_eye_position;
uniform vec3 g_eye_dir;
uniform float g_time;
uniform float g_normal_splat_size;
uniform int g_has_mirror;
uniform int g_has_refract;

in vec3 v_world_position;
in vec4 v_light_space_postion;
in vec2 v_texcoord;
in vec3 v_normal_line;
in vec3 v_tangent;
in vec2 v_texcoordReflect;
in vec3 v_eye_dir;

float reflectionFactor = 0.02037;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / vec2(1024.0,768.0);
}


vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(v_normal_line);
    vec3 Tangent = normalize(v_tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec2 texCoord = v_texcoord;
    texCoord.x +=g_time*0.01;
    texCoord.y +=g_time*0.01;
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

    vec3 waterColor = texture2D(g_diffuse_texture, v_texcoord).xyz;
    vec3 reflectColor = vec3(0,0,0);
    vec3 refractColor = vec3(0,0,0);

    const float FresnelPower=5.0;
    const float F=0.02037;
    normalize(v_eye_dir);
    float fastFresnel = F + (1.0-F) * pow(1.0 - clamp(dot(-g_eye_dir,vec3(0,1,0)),0.0,1.0),FresnelPower);


    if(g_has_mirror > 0)
    {
        vec2 a = CalcTexCoord()+normal.xz*0.03;
        a.x = clamp(a.x,0.0,1.0);
        a.y = clamp(a.y,0.0,1.0);
        reflectColor = texture2D(g_mirror_map,a).xyz;
    }

    if(g_has_refract > 0)
    {
        vec2 a = CalcTexCoord()+normal.xz*0.03;
        a.x = clamp(a.x,0.0,1.0);
        a.y = clamp(a.y,0.0,1.0);
        refractColor = texture2D(g_refract_map,a).xyz;
    }

    DiffuseOut = waterColor + reflectColor*fastFresnel + refractColor*(1.0 -fastFresnel);

    NormalOut = normal;

	return;
}

