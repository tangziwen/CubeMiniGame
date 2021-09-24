layout(set = 0, binding = 0) uniform UniformBufferObjectMat 
{
	vec4 TU_color;
	float fog_near;
	float fog_far;
	float fog_height_min;
	float fog_height_max;
	vec3 fog_color;
	vec2 TU_winSize;
	vec3 TU_camPos;
	mat4 TU_viewProjectInverted;
	vec4 TU_camInfo;
} t_shaderUnifom;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	mat4 VP;
} t_ObjectUniform;

layout(set = 0, binding = 1) uniform sampler2D RT_albedo;
layout(set = 0, binding = 2) uniform sampler2D RT_position;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_mix;
layout(set = 0, binding = 5) uniform sampler2D RT_depth;
layout(set = 0, binding = 6) uniform sampler2D RT_SceneCopy;
layout(set = 0, binding = 7) uniform sampler2D RT_Ao;

layout(set = 0, binding = 8) uniform sampler2D environmentMap;
layout(set = 0, binding = 9) uniform sampler2D prefilterMap;
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}

float LinearizeDepth(float depth) 
{

	float near = t_shaderUnifom.TU_camInfo.x;
	float far = t_shaderUnifom.TU_camInfo.y;
    // 计算ndc坐标 这里默认glDepthRange(0,1)
    float Zndc = depth; 
    // 这里分母进行了反转
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near)); 
    return (Zeye - near)/ ( far - near);
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

float getDist( float nonLinearDepth)
{
	float near = t_shaderUnifom.TU_camInfo.x;
	float far = t_shaderUnifom.TU_camInfo.y;
	float linearDepth = LinearizeDepth(nonLinearDepth);
	return near + (far - near) * linearDepth;
}

float getFogFactorByDist(float start, float end, float dist)
{
	return clamp((dist - start)/ (end - start), 0.0, 1.0);
}


float getFogFactorEXP(float density, float dist)
{
	return 1.0 - 1.0/pow(2.71828,pow(dist * density, 2.0));
}

float getFogFactor(float depth)
{
	float fogFactor = getFogFactorByDist(t_shaderUnifom.fog_near, t_shaderUnifom.fog_far, getDist(depth));
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	float heightFactor = 1.0 - smoothstep(t_shaderUnifom.fog_height_min, t_shaderUnifom.fog_height_max, worldPos.y);
	float finalFogFactor = heightFactor * fogFactor;
	return finalFogFactor;
}
//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.8;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.45;

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

void main() 
{
	float depth = texture(RT_depth, getScreenCoord()).r;
	vec3 normal = texture(RT_normal, getScreenCoord() ).xyz;
	if(length(normal) < 0.00001)
	{
		discard;
	}
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	vec3 viewDir = normalize(worldPos - t_shaderUnifom.TU_camPos.xyz);
	vec3 reflectDir = reflect(viewDir, normalize(normal));
	reflectDir = normalize(reflectDir);
	vec3 stepWorldPos = worldPos;
	vec3 reflectColor = vec3(0, 0, 0);
	float isHit = 0.0;
	depth = LinearizeDepth(depth);
	float stepLength = 1.5;
	float stepDirection = 1.0;
	vec3 albedo = texture(RT_albedo, v_texcoord).rgb;
	vec3 mixMap = texture(RT_mix, getScreenCoord()).rgb;
	float metallic = mixMap.y;
	float Roughness = mixMap.x;
	Roughness = max(Roughness, MIN_ROUGHNESS);
	float epsilon = 0.0001;
	float NoV = max(0.0, abs(dot(normal, -viewDir)));
	//SSR
	if(depth < 1.0)
	{
		for(int i = 0; i < 32; i++)
		{

			stepWorldPos += reflectDir * stepLength * stepDirection;

			vec4 fuckPos = t_ObjectUniform.VP * vec4(stepWorldPos, 1.0);
			fuckPos.xyz /= fuckPos.w;
			vec2 newUV = fuckPos.xy;
			newUV = newUV * 0.5 + vec2(0.5);
			float currDepth = texture(RT_depth, newUV).r;
			currDepth = LinearizeDepth(currDepth);
			fuckPos.z = LinearizeDepth(fuckPos.z);
			vec3 sceneColor = texture(RT_SceneCopy, newUV).rgb;
			if(newUV.x > 1.0 || newUV.x < 0.0 || newUV.y > 1.0 || newUV.y < 0.0)
			{
				break;
			}

			if(fuckPos.z > currDepth) //cross over?
			{
				if(abs(fuckPos.z - currDepth) < epsilon) // good enough?
				{
					reflectColor = sceneColor;
					float faceFade = NoV;
					isHit = clamp(1.0 - (i / 31.0) * 0.8, 0.0, 1.0);
					isHit *= (1.0 - Roughness);
					break;
				}
				else //trace back
				{
					stepLength *= 0.5;
					stepDirection = -1.0;
					//放水
					epsilon += 0.00002;
					epsilon = min(epsilon, 0.0002);
				}
			}
			else // make larger step.
			{
				stepLength *= 1.5;
				stepDirection = 1.0;
			}
		}
	}
	//IBL
	float AO = texture(RT_Ao, getScreenCoord() ).x;
	vec3 F0 = vec3(0.08); 
	F0 = mix(F0, albedo, metallic);
	vec3 diffuseColor = albedo - albedo * metallic;
	const float MAX_REFLECTION_LOD = 8.0;
	vec3 reflectionVector = normalize(reflect(viewDir, normal));

	vec3 irradiance = dualParobolidSampleLOD(environmentMap, normal, 0).rgb;
	vec3 prefilteredColor = dualParobolidSampleLOD(prefilterMap, reflectionVector,Roughness *MAX_REFLECTION_LOD).rgb;
	irradiance = pow(irradiance, vec3(2.2));
	prefilteredColor = pow(prefilteredColor, vec3(2.2));
	vec3 ambientDiffuse = diffuseColor * irradiance;
	vec3 ambientSpecular =  EnvBRDFApprox(F0, Roughness, NoV) * prefilteredColor;
	vec3 ambient = (ambientDiffuse * AO + mix(ambientSpecular, F0 * reflectColor, isHit) * AO);

	out_Color = vec4(ambient, 0.0 );
}