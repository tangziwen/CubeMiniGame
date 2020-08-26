layout(set = 0, binding = 0) uniform UniformBufferObjectMat 
{
	vec4 TU_color;
	float fog_near;
	float fog_far;
	float fog_height_min;
	float fog_height_max;
	vec3 fog_color;
	vec2 TU_winSize;
	mat4 TU_viewProjectInverted;
} t_shaderUnifom;



layout(set = 0, binding = 1) uniform sampler2D RT_albedo;
layout(set = 0, binding = 2) uniform sampler2D RT_position;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_mix;
layout(set = 0, binding = 5) uniform sampler2D RT_depth;

layout(set = 0, binding = 6) uniform sampler2D environmentMap;
layout(set = 0, binding = 7) uniform sampler2D prefilterMap;
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}

float LinearizeDepth(float near, float far, float depth) 
{
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

float getDist(float near, float far, float nonLinearDepth)
{
	float linearDepth = LinearizeDepth(near, far, nonLinearDepth);
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
	float fogFactor = getFogFactorByDist(t_shaderUnifom.fog_near, t_shaderUnifom.fog_far, getDist(0.1, 200, depth));
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	float heightFactor = 1.0 - smoothstep(t_shaderUnifom.fog_height_min, t_shaderUnifom.fog_height_max, worldPos.y);
	float finalFogFactor = heightFactor * fogFactor;
	return finalFogFactor;
}
//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.8;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.45;

void main() 
{
	float depth = texture(RT_depth, getScreenCoord()).r;
	float fogFactor = getFogFactor(depth);
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	out_Color = vec4(t_shaderUnifom.fog_color.rgb, fogFactor);
}