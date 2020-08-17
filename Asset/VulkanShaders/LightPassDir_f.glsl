layout(set = 0, binding = 1) uniform sampler2D RT_albedo;
layout(set = 0, binding = 2) uniform sampler2D RT_position;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_mix;
layout(set = 0, binding = 5) uniform sampler2D RT_depth;


layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;


layout(set = 1, binding = 0) uniform UniformBufferObjectMat 
{
	vec4 TU_color;
	vec3 TU_camPos;
	mat4 TU_viewProjectInverted;
	vec2 TU_winSize;
	vec3 TU_sunDirection;
	vec3 TU_sunColor;
} t_shaderUnifom;


#define Square(x) (x*x)
#define PI (3.14159264)
#define MIN_ROUGHNESS (0.04)

vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;
	return tmp;
}

vec4 getWorldPosFromDepth()
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = v_texcoord * 2.0 - 1.0;
  clipSpaceLocation.z = texture(RT_depth, v_texcoord).x;
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
	return (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor + albedo * 0.25;
}
void main() 
{
	vec3 albedo = texture(RT_albedo, v_texcoord).rgb;
	vec3 normal = texture(RT_normal, v_texcoord).rgb;
	vec3 mix = texture(RT_mix, v_texcoord).rgb;
	float metallic = mix.y;
	float roughness = mix.x;
	vec4 worldPos = getWorldPosFromDepth();
	vec3 worldView = normalize(t_shaderUnifom.TU_camPos.xyz - worldPos.xyz);
	vec3 resultColor = calculateLightPBR(albedo, metallic, normalize(normal), normalize(t_shaderUnifom.TU_sunDirection), t_shaderUnifom.TU_sunColor, normalize(worldView), roughness, 1);
	out_Color = vec4(resultColor, 1.0);
}
