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
uniform vec3 g_eye_position;
uniform vec4 g_clip_plane;
uniform int g_is_enable_clip;

in vec3 v_world_position;
in vec4 v_light_space_postion;
in vec2 v_texcoord;
in vec3 v_normal_line;
in vec3 v_tangent;

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(v_normal_line);
    vec3 Tangent = normalize(v_tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture2D(g_normal_map, v_texcoord).xyz;
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
    if(g_is_enable_clip > 0)
    {
        vec3 planeNormal = g_clip_plane.xyz;
        float dist = dot(planeNormal,v_world_position) - g_clip_plane.w;
        if(dist < 0.0)
        {
            discard;
        }
    }
	DiffuseOut      = texture2D(g_diffuse_texture, v_texcoord).xyz;	
	if(g_has_normal_map > 0)
	{
		NormalOut = CalcBumpedNormal();
	}else
	{
		NormalOut = normalize(v_normal_line);
	}
	return;
}

