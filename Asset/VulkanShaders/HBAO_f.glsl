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
	vec4 TU_ProjInfo;
	vec4 TU_RadiusInfo;// radius, radius*radius, radiusToscreen
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

layout(set = 0, binding = 7) uniform sampler2D environmentMap;
layout(set = 0, binding = 8) uniform sampler2D prefilterMap;
layout(set = 0, binding = 9) uniform sampler2D jitterTex;
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;

#define NUM_DIRECTIONS 8
#define NUM_STEPS 4
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}
vec2 getJitterScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= vec2(8 , 8);//vec2(640, 480);
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


vec3 UVToView(vec2 uv, float eye_z)
{
  return vec3((uv * t_shaderUnifom.TU_ProjInfo.xy + t_shaderUnifom.TU_ProjInfo.zw) * (eye_z), eye_z);
}

float getDist( float nonLinearDepth)
{
	float near = t_shaderUnifom.TU_camInfo.x;
	float far = t_shaderUnifom.TU_camInfo.y;
	float linearDepth = LinearizeDepth(nonLinearDepth);
	return near + (far - near) * linearDepth;
}

vec3 FetchViewPos(vec2 UV)
{
  float nonlinearZ = textureLod(RT_depth,UV,0).x;
  float ViewDepth = getDist(nonlinearZ);
  return UVToView(UV, ViewDepth);
}


vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
{
  vec3 V1 = Pr - P;
  vec3 V2 = P - Pl;
  return (dot(V1,V1) < dot(V2,V2)) ? V1 : V2;
}

vec3 ReconstructNormal(vec2 UV, vec3 P)
{
  vec3 Pr = FetchViewPos(UV + vec2(1.0 / t_shaderUnifom.TU_winSize.x, 0));
  vec3 Pl = FetchViewPos(UV + vec2(1.0 / -t_shaderUnifom.TU_winSize.x, 0));
  vec3 Pt = FetchViewPos(UV + vec2(0, 1.0 / t_shaderUnifom.TU_winSize.y));
  vec3 Pb = FetchViewPos(UV + vec2(0, 1.0 / -t_shaderUnifom.TU_winSize.y));
  return normalize(cross(MinDiff(P, Pr, Pl), MinDiff(P, Pt, Pb)));
}



//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.8;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.45;

#define PI (3.14159264)
#define MIN_ROUGHNESS (0.04)

//----------------------------------------------------------------------------------
vec2 RotateDirection(vec2 Dir, vec2 CosSin)
{
  return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
              Dir.x*CosSin.y + Dir.y*CosSin.x);
}

float Falloff(float DistanceSquare)
{
  // 1 scalar mad instruction
  return DistanceSquare * (-1.0 / t_shaderUnifom.TU_RadiusInfo.y)  + 1.0;
}

#define NOV_Bias 0.15
float computeAO(vec3 shadingPoint, vec3 marchingPoint, vec3 shadingPointNormal)
{
	
	vec3 V = marchingPoint - shadingPoint;
	float VdotV = dot(V, V);
	float NdotV = dot(shadingPointNormal, V) * (1.0/sqrt(VdotV));
	return clamp(NdotV - NOV_Bias, 0, 1)* clamp(Falloff(VdotV),0,1);
}

void main() 
{
	float depth = texture(RT_depth, getScreenCoord()).r;
	vec3 normal = texture(RT_normal, getScreenCoord() ).xyz;
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	vec3 viewPos = FetchViewPos(getScreenCoord()).xyz;
	vec3 normalView = ReconstructNormal(getScreenCoord(), viewPos);
	
	const float Alpha = 2.0 * PI / NUM_DIRECTIONS;
	float AO = 0;
	vec2 baseUV = getScreenCoord();
	for (float DirectionIndex = 0; DirectionIndex < NUM_DIRECTIONS; ++DirectionIndex)
	{
		float Angle = Alpha * DirectionIndex;

		// Compute normalized 2D direction
		vec3 Rand = texture(jitterTex, getJitterScreenCoord() ).xyz;
		Rand.xy = Rand.xy * 2.0 - 1.0;
		vec2 Direction = RotateDirection(vec2(cos(Angle), sin(Angle)), Rand.xy);
		float pixelRaidusMax = 300;
		//note. raidusTrue / heightTrue = raidusPixel / half winsize;
		float pixelRadius = (t_shaderUnifom.TU_RadiusInfo.x * t_shaderUnifom.TU_winSize.y * 0.5) / (t_shaderUnifom.TU_RadiusInfo.z * viewPos.z); //(t_shaderUnifom.TU_RadiusInfo.x /(t_shaderUnifom.TU_RadiusInfo.z * -viewPos.z)) *(t_shaderUnifom.TU_winSize.y * 0.5);
		pixelRadius = min(pixelRaidusMax, pixelRadius);
		vec2 stepUV = baseUV;
		for (float StepIndex = 0; StepIndex < NUM_STEPS; ++StepIndex)
		{
			stepUV += Direction * (pixelRadius / NUM_STEPS)* (1.0 / t_shaderUnifom.TU_winSize.xy) ;
			//float depthA = texture(RT_depth, stepUV).r;
			vec3 viewPosA = FetchViewPos(stepUV);//getWorldPosFromDepth(depthA).xyz;
			AO += computeAO(viewPos, viewPosA, normalView);
		}
	}
	AO *= 1.0 / (NUM_DIRECTIONS * NUM_STEPS);
	out_Color = vec4(vec3(1, 1, 1) * (1 - clamp(AO * 2.0, 0.0, 1.0)), 1.0 );
}