struct DirectionalLight
{
    vec3 color;
    vec3 direction;
	float intensity;
};
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision highp float;
#endif
struct AmbientLight
{
    vec3 color;
    float intensity;
};
const int NUM_CASCADES = 3;
uniform float F0 = 0.8;
uniform float K = 0.7;
uniform float Gamma = 1.1;

uniform float fog_near;
uniform float fog_far;
uniform vec4 fog_color;
uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_posBuffer;
uniform sampler2D TU_normalBuffer;
uniform sampler2D TU_GBUFFER4;
uniform sampler2D TU_Depth;
uniform sampler2D TU_ShadowMap[NUM_CASCADES];

uniform mat4 TU_LightVP[NUM_CASCADES];
uniform float TU_ShadowMapEnd[NUM_CASCADES];
uniform mat4 TU_viewProjectInverted;
uniform mat4 TU_viewInverted;
uniform vec2 TU_winSize;
uniform vec3 TU_camPos;


uniform DirectionalLight gDirectionalLight;
uniform AmbientLight gAmbientLight;

#if CUBE_MAP_IBL
uniform samplerCube     environmentMap;
uniform samplerCube     prefilterMap;
#else
uniform sampler2D     environmentMap;
uniform sampler2D     prefilterMap;
#endif

varying vec2 v_texcoord;

#define Square(x) (x*x)
#define PI (3.14159264)
#define MIN_ROUGHNESS (0.04)

vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= TU_winSize;
	return tmp;
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
	float nonLinearDepth = texture2D(TU_Depth, getScreenCoord()).r;
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
	denom = max(denom, 0.0001);
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
	return SpecularColor * AB.x + AB.y;
}

vec4 dualParobolidSampleLOD(sampler2D tex, vec3 r, float LODLevel)
{
	vec2 uv;
	uv.x = atan(r.x,r.z) / PI;
	uv.y = r.y;
	uv= uv* 0.5 + 0.5;
	return textureLod(tex, uv, LODLevel);
}

vec4 dualParobolidSample(sampler2D tex, vec3 r)
{

	vec2 uv;
	uv.x = atan(r.x,r.z) / PI;
	uv.y = r.y;
	uv= uv* 0.5 + 0.5;
	return texture(tex, uv);
}

vec3 calculateLightPBR(vec3 albedo, float metallic, vec3 N, vec3 L, vec3 lightColor,vec3 V,float Roughness, float shadowFactor)
{
	L = -L;

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	float NoV = max(0.0, dot(N, V));

	// Roughness = 0.0;
	// calculate per-light radiance
	vec3 H = normalize(V + L);
	vec3 radiance     = lightColor;

	// cook-torrance brdf
	float NDF = DistributionGGX(N, H, clamp(Roughness, MIN_ROUGHNESS, 1.0));
	
	float G   = GeometrySmith(N, V, L, Roughness);      
	vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 nominator    = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0); 
	vec3 specular     = nominator / max(denominator, 0.001);

	// add to outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);

	//IBL 
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 reflectionVector = normalize(reflect(-V, N));

	#if CUBE_MAP_IBL
		vec3 irradiance = textureLod(environmentMap, N, 0).rgb;
		
		vec3 prefilteredColor = textureLod(prefilterMap, reflectionVector, Roughness *MAX_REFLECTION_LOD).rgb;
	#else
		vec3 irradiance = dualParobolidSampleLOD(environmentMap, N, 0).rgb;
		vec3 prefilteredColor = dualParobolidSampleLOD(prefilterMap, reflectionVector,Roughness *MAX_REFLECTION_LOD).rgb;
	#endif
	irradiance = pow(irradiance, vec3(2.2));
	prefilteredColor = pow(prefilteredColor, vec3(2.2));
	vec3 ambientDiffuse = kD * irradiance * albedo;
	vec3 ambientSpecular =  EnvBRDFApprox(F0, Roughness, NoV) * prefilteredColor;
	vec3 ambient = (ambientDiffuse + ambientSpecular) * gAmbientLight.intensity;
	
	return (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor + ambient;
}


vec3 calculateLightLambert(vec3 normal, vec3 lightDir, vec3 lightColor,vec3 viewPos,float Roughness, float shadowFactor)
{
	vec3 diffuseResult,ambientColor;
	float specularIntensity =0.0;
	float lambert = max(0.5, dot(normal, -lightDir));

	float irradiance = min(shadowFactor, lambert);
	return lightColor * irradiance * gDirectionalLight.intensity;
}

vec4 getWorldPosFromDepth()
{
	// return vec4(texture(TU_posBuffer, v_texcoord).xyz, 1.0);
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = v_texcoord * 2.0 - 1.0;
  clipSpaceLocation.z = texture(TU_Depth, v_texcoord).x*2.0 - 1.0;
  clipSpaceLocation.w = 1.0;
  vec4 homogenousLocation = TU_viewProjectInverted * clipSpaceLocation;
  return vec4(homogenousLocation.xyz / homogenousLocation.w, 1.0);
}

float getBias(vec3 normal, vec3 dirLight)
{
	return max(0.0002 * (1.0 - dot(normal, dirLight)), 0.0008);
}

/*
const vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);
*/

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
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

#define SampleShadowMap(index, UV, result) result = texture( TU_ShadowMap[index], UV).z;
float pcf_3x3(vec2 pInLitTC, float litZ, vec2 vInvShadowMapWH, int shadowIndex)
{
	vec2 TexelPos = pInLitTC / vInvShadowMapWH - 0.5;	// bias to be consistent with texture filtering hardware
	vec2 Fraction = fract(TexelPos);
	vec2 TexelCenter = floor(TexelPos) + 0.5;	// bias to get reliable texel center content
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

float CalcShadowFactor(int index, vec4 LightSpacePos, vec3 surfaceNormal, vec3 lightDir)                                                  
{   
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  
    vec2 UVCoords;                                                                          
    UVCoords.x =0.5 * ProjCoords.x + 0.5;                                                  
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
	if (UVCoords.x < 0 || UVCoords.x > 1 || UVCoords.y < 0 || UVCoords.y > 1)
		return 1.0;
    //
	
	return clamp(1.0 - pcf_3x3(UVCoords.xy, z - getBias(surfaceNormal, lightDir), vec2(1.0 / 1024.0, 1.0 / 1024.0), index), 0.1, 1.0);
}

// knarkowicz
vec3 ACES_Knarkowicz( vec3 x )
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float s = 0.14;
    return (x*(a*x+b))/(x*(c*x+d)+s);
}

void main()
{
	vec4 Data1 = texture2D(TU_colorBuffer, v_texcoord);
	vec3 color = Data1.xyz;
	vec3 surfaceData = texture2D(TU_GBUFFER4, v_texcoord).rgb;
	vec3 pos = texture2D(TU_posBuffer, v_texcoord).xyz;
	vec4 worldPos = getWorldPosFromDepth();
	float depth = texture(TU_Depth, v_texcoord).x;
	float shadowFactor = 0.0;
	vec3 normal = normalize(texture2D(TU_normalBuffer, v_texcoord).xyz);
	for(int i = 0; i < NUM_CASCADES; i++)
	{
		if(depth < TU_ShadowMapEnd[i])
		{
			shadowFactor = CalcShadowFactor(i, TU_LightVP[i] * worldPos, normal, gDirectionalLight.direction);
			break;
		}
	}

	vec3 worldView = normalize(TU_camPos.xyz - worldPos.xyz);
	vec3 resultColor =  calculateLightPBR(color, surfaceData[1], normalize(normal), gDirectionalLight.direction, gDirectionalLight.color * gDirectionalLight.intensity, normalize(worldView), surfaceData[0], shadowFactor);

	gl_FragColor = vec4(resultColor, 1.0);
}
