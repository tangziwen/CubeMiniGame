#define NUM_CASCADES 3
layout(set = 0, binding = 0) uniform UniformBufferObjectMat 
{
	vec4 TU_color;
	vec3 TU_camPos;
	mat4 TU_viewProjectInverted;
	vec2 TU_winSize;
	vec3 TU_sunDirection;
	vec3 TU_sunColor;
	mat4 TU_LightVP[NUM_CASCADES];
	vec4 TU_ShadowMapEnd;
} t_shaderUnifom;

layout(set = 0, binding = 1) uniform sampler2D RT_albedo;
layout(set = 0, binding = 2) uniform sampler2D RT_position;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_mix;
layout(set = 0, binding = 5) uniform sampler2D RT_depth;
layout(set = 0, binding = 6) uniform sampler2D environmentMap;
layout(set = 0, binding = 7) uniform sampler2D prefilterMap;

layout(set = 0, binding = 8) uniform sampler2D TU_ShadowMap_1;
layout(set = 0, binding = 9) uniform sampler2D TU_ShadowMap_2;
layout(set = 0, binding = 10) uniform sampler2D TU_ShadowMap_3;
//layout(set = 0, binding = 8) uniform sampler2D TU_ShadowMap[3];

layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;





#define Square(x) (x*x)
#define PI (3.14159264)
#define MIN_ROUGHNESS (0.04)
vec4 dualParobolidSampleLOD(sampler2D tex, vec3 r, float LODLevel)
{
	vec2 uv;
	uv.x = atan(r.x,r.z) / PI;
	uv.y = r.y;
	uv= uv* 0.5 + 0.5;
	uv.y = 1.0 -uv.y;
	return textureLod(tex, uv, LODLevel);
}
float getBias(vec3 normal, vec3 dirLight)
{
	return 0.005;//max(0.0003 * (1.0 - dot(normal, dirLight)), 0.0008);
}
vec4 step4(vec4 value1, vec4 value2)
{
	vec4 result;
	result.x = step(value1.x, value2.x);
	result.y = step(value1.y, value2.y);
	result.z = step(value1.z, value2.z);
	result.w = step(value1.w, value2.w);
	return result;
}
#define SampleShadowMap(the_sampler, UV, result) result = texture( the_sampler, UV).r;
float pcf_3x3(vec2 pInLitTC, float litZ, vec2 vInvShadowMapWH, sampler2D shadowIndex)
{
	vec4 bias = vec4(0.005, 0.005, 0.005, 0.005);
	vec2 TexelPos = pInLitTC / vInvShadowMapWH - vec2(0.5);	// bias to be consistent with texture filtering hardware
	vec2 Fraction = fract(TexelPos);
	vec2 TexelCenter = floor(TexelPos) + vec2(0.5);	// bias to get reliable texel center content
	vec2 Sample00TexelCenter = TexelCenter - vec2(1, 1);
	
	vec4 Values0, Values1, Values2, Values3;// Valuesi in row i from left to right: x,y,z,w
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,0))*vInvShadowMapWH, Values0.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,0))*vInvShadowMapWH, Values0.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,0))*vInvShadowMapWH, Values0.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,0))*vInvShadowMapWH, Values0.w );
	Values0 = step4(Values0 + bias, vec4(litZ, litZ, litZ, litZ));
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,1))*vInvShadowMapWH, Values1.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,1))*vInvShadowMapWH, Values1.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,1))*vInvShadowMapWH, Values1.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,1))*vInvShadowMapWH, Values1.w );
	Values1 = step4(Values1 + bias, vec4(litZ, litZ, litZ, litZ));
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,2))*vInvShadowMapWH, Values2.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,2))*vInvShadowMapWH, Values2.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,2))*vInvShadowMapWH, Values2.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,2))*vInvShadowMapWH, Values2.w );
	Values2 = step4(Values2 + bias, vec4(litZ, litZ, litZ, litZ));
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,3))*vInvShadowMapWH, Values3.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,3))*vInvShadowMapWH, Values3.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,3))*vInvShadowMapWH, Values3.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,3))*vInvShadowMapWH, Values3.w );
	
	Values3 = step4(Values3 + bias, vec4(litZ, litZ, litZ, litZ));

	vec2 VerticalLerp00 = mix(vec2(Values0.x, Values1.x), vec2(Values0.y, Values1.y), Fraction.xx);
	float PCFResult00 = mix(VerticalLerp00.x, VerticalLerp00.y, Fraction.y);
	vec2 VerticalLerp10 = mix(vec2(Values0.y, Values1.y), vec2(Values0.z, Values1.z), Fraction.xx);
	float PCFResult10 = mix(VerticalLerp10.x, VerticalLerp10.y, Fraction.y);
	vec2 VerticalLerp20 = mix(vec2(Values0.z, Values1.z), vec2(Values0.w, Values1.w), Fraction.xx);
	float PCFResult20 = mix(VerticalLerp20.x, VerticalLerp20.y, Fraction.y);

	vec2 VerticalLerp01 = mix(vec2(Values1.x, Values2.x), vec2(Values1.y, Values2.y), Fraction.xx);
	float PCFResult01 = mix(VerticalLerp01.x, VerticalLerp01.y, Fraction.y);
	vec2 VerticalLerp11 = mix(vec2(Values1.y, Values2.y), vec2(Values1.z, Values2.z), Fraction.xx);
	float PCFResult11 = mix(VerticalLerp11.x, VerticalLerp11.y, Fraction.y);
	vec2 VerticalLerp21 = mix(vec2(Values1.z, Values2.z), vec2(Values1.w, Values2.w), Fraction.xx);
	float PCFResult21 = mix(VerticalLerp21.x, VerticalLerp21.y, Fraction.y);

	vec2 VerticalLerp02 = mix(vec2(Values2.x, Values3.x), vec2(Values2.y, Values3.y), Fraction.xx);
	float PCFResult02 = mix(VerticalLerp02.x, VerticalLerp02.y, Fraction.y);
	vec2 VerticalLerp12 = mix(vec2(Values2.y, Values3.y), vec2(Values2.z, Values3.z), Fraction.xx);
	float PCFResult12 = mix(VerticalLerp12.x, VerticalLerp12.y, Fraction.y);
	vec2 VerticalLerp22 = mix(vec2(Values2.z, Values3.z), vec2(Values2.w, Values3.w), Fraction.xx);
	float PCFResult22 = mix(VerticalLerp22.x, VerticalLerp22.y, Fraction.y);
	
	return (PCFResult00 + PCFResult10 + PCFResult20 + PCFResult01 + PCFResult11 + PCFResult21 + PCFResult02 + PCFResult12 + PCFResult22) * .11111;
}
float CalcShadowFactor(sampler2D depthSampler, vec4 LightSpacePos, vec3 surfaceNormal, vec3 lightDir)                                                  
{   
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x =0.5 * ProjCoords.x + 0.5;                                                  
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = ProjCoords.z;
	if (UVCoords.x < 0 || UVCoords.x > 1 || UVCoords.y < 0 || UVCoords.y > 1)
		return 1.0;
	return clamp(1.0 - pcf_3x3(UVCoords.xy, z, vec2(1.0 / 2048.0, 1.0 / 2048.0), depthSampler), 0.1, 1.0);
}
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;
	return tmp;
}

vec4 getWorldPosFromDepth(float depthValue)
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = v_texcoord * 2.0 - 1.0;
  clipSpaceLocation.z = depthValue;
  clipSpaceLocation.w = 1.0;
  vec4 homogenousLocation = t_shaderUnifom.TU_viewProjectInverted * clipSpaceLocation;
  return vec4(homogenousLocation.xyz / homogenousLocation.w, 1.0);
}

float LinearizeDepth(float near, float far, float depth) 
{
    // 计算ndc坐标 这里默认glDepthRange(0,1)
    float Zndc = depth * 2.0 - 1.0; 
    // 这里分母进行了反转
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near)); 
    return (Zeye - near)/ ( far - near);
}

float getDist(float near, float far)
{
	float nonLinearDepth = texture(RT_depth, v_texcoord).r;
	float linearDepth = LinearizeDepth(near, far, nonLinearDepth);
	return near + (far - near) * linearDepth;
}

float getFogFactor(float start, float end, float dist)
{
	return clamp((dist - start)/ (end - start), 0.0, 1.0);
}


float getFogFactorEXP(float density, float dist)
{
	return 1.0 - 1.0/pow(2.71828,pow(dist * density, 2.0));
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float Pow5( float x )
{
	float xx = x*x;
	return xx * xx * x;
}

vec3 fresnelSchlick(float VoH, vec3 specularColor)
{
	float Fc = Pow5( 1 - VoH );					// 1 sub, 3 mul
	//return Fc + (1 - Fc) * SpecularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	return clamp( 50.0 * specularColor.g, 0.0, 1.0) * Fc + (1 - Fc) * specularColor;
}


vec3 EnvBRDFApprox( vec3 SpecularColor, float Roughness, float NoV )
{
	const vec4 c0 = vec4( -1, -0.0275, -0.572, 0.022 );
	const vec4 c1 = vec4( 1, 0.0425, 1.04, -0.04 );
	vec4 r = Roughness * c0 + c1;
	float a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
	vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;
	AB.y *= clamp( 50.0 * SpecularColor.g, 0, 1);
	return SpecularColor * AB.x + AB.y;
}

float Vis_Schlick( float roughness, float NoV, float NoL )
{
	float k = (roughness * roughness) * 0.5;
	float Vis_SchlickV = NoV * (1 - k) + k;
	float Vis_SchlickL = NoL * (1 - k) + k;
	return 0.25 / ( Vis_SchlickV * Vis_SchlickL );
}

vec3 diffuseLambert(vec3 diffuseColor)
{
	return diffuseColor * (1 / PI);

}
vec3 calculateLightPBR(vec3 albedo, float metallic, vec3 N, vec3 L, vec3 lightColor,vec3 V,float Roughness, float shadowFactor)
{
	L = -L;
	vec3 F0 = vec3(0.08); 
	F0 = mix(F0, albedo, metallic);
	
	float NoV = max(0.0, abs(dot(N, V)));
	
	// calculate per-light radiance
	vec3 H = normalize(V + L);
	vec3 radiance     = lightColor;
	Roughness = max(Roughness, MIN_ROUGHNESS);
	// cook-torrance brdf
	float NDF = DistributionGGX(N, H, Roughness);
	
	//float G   = GeometrySmith(N, V, L, Roughness); 
	float G   = Vis_Schlick(Roughness, NoV, dot(N, L));     
	vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       

	vec3 diffuseColor = albedo - albedo * metallic;
	vec3 specular     = vec3(NDF)* G * F;
	// add to outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);

	//IBL 
	const float MAX_REFLECTION_LOD = 8.0;
	vec3 reflectionVector = normalize(reflect(-V, N));

	vec3 irradiance = dualParobolidSampleLOD(environmentMap, N, 0).rgb;
	vec3 prefilteredColor = dualParobolidSampleLOD(prefilterMap, reflectionVector,Roughness *MAX_REFLECTION_LOD).rgb;
	irradiance = pow(irradiance, vec3(2.2));
	prefilteredColor = pow(prefilteredColor, vec3(2.2));
	vec3 ambientDiffuse = diffuseColor * irradiance;
	vec3 ambientSpecular =  EnvBRDFApprox(F0, Roughness, NoV) * prefilteredColor;
	vec3 ambient = (ambientDiffuse + ambientSpecular);
	return (diffuseLambert(diffuseColor) + specular) * radiance * NdotL * shadowFactor;
}
void main() 
{
	vec3 albedo = texture(RT_albedo, v_texcoord).rgb;
	vec3 normal = texture(RT_normal, v_texcoord).rgb;
	float depth = texture(RT_depth, v_texcoord).x;
	vec3 mixMap = texture(RT_mix, v_texcoord).rgb;
	float metallic = mixMap.y;
	float roughness = mixMap.x;
	float dFactor = step(depth, 0.99999);
	vec4 worldPos = getWorldPosFromDepth(depth);
	vec3 worldView = normalize(t_shaderUnifom.TU_camPos.xyz - worldPos.xyz);
	float shadowFactor = 1.0;
	
	if(depth < t_shaderUnifom.TU_ShadowMapEnd[0])
	{
		shadowFactor = CalcShadowFactor(TU_ShadowMap_1, t_shaderUnifom.TU_LightVP[0] * worldPos, normal, t_shaderUnifom.TU_sunDirection);
	}
	else
	if(depth < t_shaderUnifom.TU_ShadowMapEnd[1])
	{
		shadowFactor = CalcShadowFactor(TU_ShadowMap_2, t_shaderUnifom.TU_LightVP[1] * worldPos, normal, t_shaderUnifom.TU_sunDirection);
	}
	else
	if(depth < t_shaderUnifom.TU_ShadowMapEnd[2])
	{
		shadowFactor = CalcShadowFactor(TU_ShadowMap_3, t_shaderUnifom.TU_LightVP[2] * worldPos, normal, t_shaderUnifom.TU_sunDirection);
	}
	/*
	for(int i = 0; i < NUM_CASCADES; i++)
	{
		if(depth < t_shaderUnifom.TU_ShadowMapEnd[i])
		{
			shadowFactor = CalcShadowFactor(i, t_shaderUnifom.TU_LightVP[i] * worldPos, normal, t_shaderUnifom.TU_sunDirection);
			break;
		}
	}
	*/
	vec3 resultColor = calculateLightPBR(albedo, metallic, normalize(normal), normalize(t_shaderUnifom.TU_sunDirection), t_shaderUnifom.TU_sunColor, normalize(worldView), roughness, shadowFactor);
	out_Color = vec4(mix(vec3(0,0,0), resultColor, vec3(dFactor)), 1.0);
}
