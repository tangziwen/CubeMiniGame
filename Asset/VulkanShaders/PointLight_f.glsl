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
layout(set = 1, binding = 0) uniform UniformBufferObject {
	mat4 mvp;
	vec4 LightPos;
	vec4 LightColor;
} t_ObjectUniform;
layout(set = 0, binding = 1) uniform sampler2D RT_albedo;
layout(set = 0, binding = 2) uniform sampler2D RT_position;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_mix;
layout(set = 0, binding = 5) uniform sampler2D RT_depth;

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
	vec2 TexelPos = pInLitTC / vInvShadowMapWH - vec2(0.5);	// bias to be consistent with texture filtering hardware
	vec2 Fraction = fract(TexelPos);
	vec2 TexelCenter = floor(TexelPos) + vec2(0.5);	// bias to get reliable texel center content
	vec2 Sample00TexelCenter = TexelCenter - vec2(1, 1);
	
	vec4 Values0, Values1, Values2, Values3;// Valuesi in row i from left to right: x,y,z,w
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,0))*vInvShadowMapWH, Values0.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,0))*vInvShadowMapWH, Values0.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,0))*vInvShadowMapWH, Values0.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,0))*vInvShadowMapWH, Values0.w );
	Values0 = step4(Values0, vec4(litZ, litZ, litZ, litZ));
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,1))*vInvShadowMapWH, Values1.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,1))*vInvShadowMapWH, Values1.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,1))*vInvShadowMapWH, Values1.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,1))*vInvShadowMapWH, Values1.w );
	Values1 = step4(Values1, vec4(litZ, litZ, litZ, litZ));
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,2))*vInvShadowMapWH, Values2.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,2))*vInvShadowMapWH, Values2.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,2))*vInvShadowMapWH, Values2.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,2))*vInvShadowMapWH, Values2.w );
	Values2 = step4(Values2, vec4(litZ, litZ, litZ, litZ));
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(0,3))*vInvShadowMapWH, Values3.x );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(1,3))*vInvShadowMapWH, Values3.y );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(2,3))*vInvShadowMapWH, Values3.z );
	SampleShadowMap(shadowIndex, (Sample00TexelCenter+vec2(3,3))*vInvShadowMapWH, Values3.w );
	Values3 = step4(Values3, vec4(litZ, litZ, litZ, litZ));

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
	return clamp(1.0 - pcf_3x3(UVCoords.xy, z, vec2(1.0 / 1024.0, 1.0 / 1024.0), depthSampler), 0.1, 1.0);
}
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;
	return tmp;
}

vec4 getWorldPosFromDepth(float depthValue, vec2 uv)
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = uv * 2.0 - 1.0;
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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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

vec3 calculateLightPBR(vec3 albedo, float metallic, vec3 N, vec3 L, vec3 lightColor,vec3 V,float Roughness, float shadowFactor)
{
	L = -L;
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);
	
	float NoV = max(0.0, dot(N, V));
	
	// calculate per-light radiance
	vec3 H = normalize(V + L);
	vec3 radiance     = lightColor;
	Roughness = max(Roughness, MIN_ROUGHNESS);
	// cook-torrance brdf
	float NDF = DistributionGGX(N, H, Roughness);
	
	float G   = GeometrySmith(N, V, L, Roughness);      
	vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 nominator    = vec3(NDF)* G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0); 
	vec3 specular     = nominator / max(denominator, 0.001);
	// add to outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);

	//IBL 
	return (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor;
}
void main() 
{
	vec2 uv = getScreenCoord();
	vec3 albedo = texture(RT_albedo, uv).rgb;
	vec3 normal = texture(RT_normal, uv).rgb;
	float depth = texture(RT_depth, uv).x;
	vec3 mixMap = texture(RT_mix, uv).rgb;
	float metallic = mixMap.y;
	float roughness = mixMap.x;
	float dFactor = step(depth, 0.99999);
	vec4 worldPos = getWorldPosFromDepth(depth, uv);
	vec3 worldView = normalize(t_shaderUnifom.TU_camPos.xyz - worldPos.xyz);
	float shadowFactor = 1.0;
	float dist = length(worldPos.xyz - t_ObjectUniform.LightPos.xyz);
	float linearFactor = 1.0 - clamp(dist / t_ObjectUniform.LightPos.w, 0, 1);
	vec3 resultColor = calculateLightPBR(albedo, metallic, normalize(normal), normalize(worldPos.xyz - t_ObjectUniform.LightPos.xyz), t_ObjectUniform.LightColor.xyz * pow(linearFactor, 2.0), normalize(worldView), roughness, shadowFactor);
	out_Color = vec4(mix(vec3(0,0,0), resultColor, vec3(dFactor)), 1.0);
}
