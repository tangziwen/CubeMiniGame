uniform float F0 = 0.8;
uniform float K = 0.7;
uniform float Gamma = 1.4;

uniform sampler2D TU_colorBuffer;
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
const int MAX_KERNEL_SIZE = 64;
const int MAX_NOISE_SIZE = 16;
uniform vec3 gKernel[MAX_KERNEL_SIZE];
uniform sampler2D gNoise;

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
	vec4 color = texture2D(TU_colorBuffer, v_texcoord);
	float fogFactor = getFogFactor();
	color = color * (1.0 - fogFactor) + fogFactor * vec4(1.0, 1.0, 1.0, 1.0);

	//determine center position
	vec2 position = (gl_FragCoord.xy / TU_winSize.xy) - vec2(0.5);
	
	//determine the vector length of the center position
	float len = length(position);
	
	//use smoothstep to create a smooth vignette
	float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);
	
	//apply the vignette with 50% opacity
	color.rgb = mix(color.rgb, color.rgb * vignette, 0.3);

	vec2 noiseScale = vec2(TU_winSize.x/4.0, TU_winSize.y/4.0);
    vec3 fragPos = (TU_view * vec4(texture(TU_positionBuffer, v_texcoord).xyz, 1.0)).rgb;
    vec3 normal = normalize((TU_view * vec4(texture(TU_normalBuffer, v_texcoord).rgb, 0.0)).rgb);
    vec3 randomVec = normalize(texture(gNoise, v_texcoord * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
	float radius = 1.0;
	float bias = 0.025;
	float occlusion = 0.0;
    for(int i = 0; i < MAX_KERNEL_SIZE; ++i)
    {
        // get sample position
        vec3 sampleValue = TBN * gKernel[i]; // from tangent to view-space
        sampleValue = fragPos + sampleValue * AO_distant; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sampleValue, 1.0);
        offset = TU_Project * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
		offset.x = clamp(offset.x, 0.0, 1.0);
		offset.y = clamp(offset.y, 0.0, 1.0);
        // get sample depth
        float sampleDepth = (TU_view * vec4(texture(TU_positionBuffer, offset.xy).xyz, 1.0)).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, AO_distant / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sampleValue.z + AO_bias ? 1.0 : 0.0) * rangeCheck * AO_strength;
    }
	occlusion = 1.0 - (occlusion / MAX_KERNEL_SIZE);
	gl_FragColor = vec4(occlusion);
}
