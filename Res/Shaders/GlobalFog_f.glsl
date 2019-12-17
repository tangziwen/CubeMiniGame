

uniform float F0 = 0.8;
uniform float K = 0.7;
uniform float Gamma = 1.4;

uniform sampler2D TU_Depth;
uniform vec2 TU_winSize;
uniform float fog_near;
uniform float fog_far;

uniform float fog_height_min;
uniform float fog_height_max;
uniform mat4 TU_viewProjectInverted;
uniform mat4 TU_Project;
uniform mat4 TU_view;
uniform float AO_distant;
uniform float AO_strength;
uniform sampler2D TU_normalBuffer;
uniform sampler2D TU_positionBuffer;
uniform vec3 fog_color;
const int MAX_KERNEL_SIZE = 64;
const int MAX_NOISE_SIZE = 16;
uniform vec3 gKernel[MAX_KERNEL_SIZE];
uniform sampler2D gNoise;
#define E 0.000001
uniform float AO_bias;


varying vec2 v_texcoord;

float LinearizeDepth(float near, float far, float depth) 
{
    // 计算ndc坐标 这里默认glDepthRange(0,1)
    float Zndc = depth * 2.0 - 1.0; 
    // 这里分母进行了反转
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near)); 
    return (Zeye - near)/ ( far - near);
}

vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= TU_winSize;
	return tmp;
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

float getDist(float near, float far)
{
	float nonLinearDepth = texture2D(TU_Depth, getScreenCoord()).r;
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

float getFogFactor()
{
	float fogFactor = getFogFactorByDist(fog_near, fog_far, getDist(0.1, 200));
	vec3 worldPos = getWorldPosFromDepth().xyz;
	float heightFactor = 1.0 - smoothstep(fog_height_min, fog_height_max, worldPos.y);
	float finalFogFactor = heightFactor * fogFactor;
	return finalFogFactor;
}
//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.8;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.45;

void main()
{
	float depth = texture(TU_Depth, getScreenCoord()).r;
	float nonLinearDepth = texture2D(TU_Depth, getScreenCoord()).r * 0.5;
	float fogFactor = getFogFactor();
	vec3 worldPos = getWorldPosFromDepth().xyz;
	gl_FragColor = vec4(fog_color.rgb, fogFactor);
}
