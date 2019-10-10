#version 130
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

varying vec2 v_texcoord;

#define Square(x) (x*x)
#define PI (3.14159264)
#define MIN_ROUGHNESS (0.08)

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

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
// [Lagarde 2012, "Spherical Gaussian approximation for Blinn-Phong, Phong and Fresnel"]
vec3 F_Schlick( vec3 SpecularColor, float VoH )
{
	// Anything less than 2% is physically impossible and is instead considered to be shadowing 
	return SpecularColor + ( clamp( 50.0 * SpecularColor.g , 0.0, 1.0) - SpecularColor ) * exp2( (-5.55473 * VoH - 6.98316) * VoH );

	//float Fc = exp2( (-5.55473 * VoH - 6.98316) * VoH );	// 1 mad, 1 mul, 1 exp 	//float Fc = pow( 1 - VoH, 5 );
	//return Fc + (1 - Fc) * SpecularColor;					// 1 add, 3 mad
}

// Tuned to match behavior of Vis_Smith
// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float G_Schlick( float Roughness, float NoV, float NoL )
{
	float k = Square( Roughness ) * 0.5;
	float Vis_SchlickV = NoV * (1 - k) + k;
	float Vis_SchlickL = NoL * (1 - k) + k;
	return 0.25 / mix(0.05, 1, Vis_SchlickV * Vis_SchlickL);
}


// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX( float Roughness, float NoH )
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float d = ( NoH * m2 - NoH ) * NoH + 1;	// 2 mad
	return m2 / ( PI*d*d );					// 2 mul, 1 rcp
}


// Vis = G / (4*NoL*NoV)
float GeometricVisibility( float Roughness, float NoV, float NoL, float VoH )
{
	return G_Schlick( Roughness, NoV, NoL );
}

vec3 PbrBRDF_SPEC(in vec3 L, in vec3 N, in vec3 V, in float NoV, in float NoL,
	in vec3 SpecularColor, in float Roughness)
{
	vec3 H = normalize(V + L);
	float NoH = clamp( dot(N, H), 0.0, 1.0);
	float VoH = clamp( dot(V, H), 0.0, 1.0);

	float VoL = clamp( dot(V, L), 0.0, 1.0);
	float LoH = clamp( dot(L, H), 0.0, 1.0);

	Roughness = max(MIN_ROUGHNESS, Roughness);

	// return EvaluateSphericalAreaGGX(NoL, NoV, VoL, LoH, 3, 10, Roughness * Roughness, Roughness, SpecularColor);
	
	float D = D_GGX(Roughness, NoH );
	// half D = BRDFSpecOverPI(NoH, VoH, max(MIN_ROUGHNESS, Roughness));
	float G = GeometricVisibility( Roughness, NoV, NoL, VoH);
	// G = G_Implicit();
	vec3 F = F_Schlick( SpecularColor, VoH );
	// F = F_None(SpecularColor);

	return D*G*F;
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

vec3 calculateLightPBR(vec3 albedo, float metallic, vec3 N, vec3 L, vec3 lightColor,vec3 V,float Roughness)
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
	float NDF = DistributionGGX(N, H, Roughness);        
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
	return (kD * albedo / PI + specular) * radiance * NdotL + gAmbientLight.color * gAmbientLight.intensity * albedo;
}


vec3 calculateLightLambert(vec3 normal, vec3 lightDir, vec3 lightColor,vec3 viewPos,float Roughness, float shadowFactor)
{
	vec3 diffuseResult,ambientColor;
	float specularIntensity =0.0;
	float lambert = max(0.5, dot(normal, -lightDir));

	float irradiance = min(shadowFactor, lambert);
	return lightColor * irradiance * gDirectionalLight.intensity;
}


vec3 calculateLightBlinnPhong(vec3 normal, vec3 lightDir, vec3 lightColor,vec3 viewPos,float Roughness)
{
	vec3 diffuseColor,ambientColor;
	float specularIntensity =0.0;
	float irradiance = max(0.0,dot(normal, lightDir));
	if(irradiance > 0.0)
	{
		vec3 viewDir = normalize(-viewPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		specularIntensity = pow( max( 0, dot(halfDir, normal)), 2.0);
	}
	return lightColor * specularIntensity;
	return lightColor * irradiance + gAmbientLight.color * gAmbientLight.intensity;
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
	return max(0.02 * (1.0 - dot(normal, dirLight)), 0.005);
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
	
	return clamp(1.0 - pcf_3x3(UVCoords.xy, z - getBias(surfaceNormal, lightDir), vec2(1.0 / 512.0, 1.0 / 512.0), index), 0.5, 1.0);
	/*
	float visibility=0.0;
	for (int i=0;i<16;i++){
		int DiskIdx = int(16.0*random(gl_FragCoord.xyy, i))%16;
		if ( texture( TU_ShadowMap[index], UVCoords.xy + poissonDisk[DiskIdx] / 1024.0 * 1.5 ).z  <  z- getBias(surfaceNormal, lightDir))
		{
			visibility += 0.3;
		}
		else
		{
			visibility += 1.0;
		}
	}
	visibility /= 16.0;
	return visibility;
	*/
	/*
	float depthInTex = texture(TU_ShadowMap[index], UVCoords).x;
    if (depthInTex + getBias(surfaceNormal, lightDir) < z)
        return 0.5;
    else  
        return 1.0;
	*/
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

	vec3 worldView = normalize(TU_camPos.xyz - pos.xyz);
	vec3 resultColor =  calculateLightPBR(color, surfaceData[1], normal, gDirectionalLight.direction, gDirectionalLight.color * gDirectionalLight.intensity, worldView, surfaceData[0]);

	gl_FragColor = vec4(resultColor, 1.0);
}
