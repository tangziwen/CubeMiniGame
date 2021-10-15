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
	mat4 TU_VP;
	vec4 TU_camInfo;
	int TU_FrameIndex;
} t_shaderUnifom;


layout(set = 0, binding = 1) uniform sampler2D RT_SceneCopy;
layout(set = 0, binding = 2) uniform sampler2D RT_depth;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_albedo;


layout(set = 0, binding = 7) uniform sampler2D RT_Ao;

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

vec2 UniformSampleDiskConcentric( vec2 E )
{
	vec2 p = 2 * E - 1;
	float Radius;
	float Phi;
	if( abs( p.x ) > abs( p.y ) )
	{
		Radius = p.x;
		Phi = (PI/4) * (p.y / p.x);
	}
	else
	{
		Radius = p.y;
		Phi = (PI/2) - (PI/4) * (p.x / p.y);
	}
	return vec2( Radius * cos( Phi ), Radius * sin( Phi ) );
}

vec2 Hammersley16( uint Index, uint NumSamples, uvec2 Random )
{
#if 1
	// ReverseBits32 cann't be compiled for gl, use table instead...
	const uint ReverseBit32Table[128] =uint[128]
	(
		 0, 			 2147483648, 	 1073741824, 	 3221225472, 	 536870912, 	 2684354560, 	 1610612736, 	 3758096384, 	 268435456, 	 2415919104, 	
		 1342177280, 	 3489660928, 	 805306368, 	 2952790016, 	 1879048192, 	 4026531840, 	 134217728, 	 2281701376, 	 1207959552, 	 3355443200, 	
		 671088640, 	 2818572288, 	 1744830464, 	 3892314112, 	 402653184, 	 2550136832, 	 1476395008, 	 3623878656, 	 939524096, 	 3087007744, 	
		 2013265920, 	 4160749568, 	 67108864, 	 2214592512, 	 1140850688, 	 3288334336, 	 603979776, 	 2751463424, 	 1677721600, 	 3825205248, 	
		 335544320, 	 2483027968, 	 1409286144, 	 3556769792, 	 872415232, 	 3019898880, 	 1946157056, 	 4093640704, 	 201326592, 	 2348810240, 	
		 1275068416, 	 3422552064, 	 738197504, 	 2885681152, 	 1811939328, 	 3959422976, 	 469762048, 	 2617245696, 	 1543503872, 	 3690987520, 	
		 1006632960, 	 3154116608, 	 2080374784, 	 4227858432, 	 33554432, 	 2181038080, 	 1107296256, 	 3254779904, 	 570425344, 	 2717908992, 	
		 1644167168, 	 3791650816, 	 301989888, 	 2449473536, 	 1375731712, 	 3523215360, 	 838860800, 	 2986344448, 	 1912602624, 	 4060086272, 	
		 167772160, 	 2315255808, 	 1241513984, 	 3388997632, 	 704643072, 	 2852126720, 	 1778384896, 	 3925868544, 	 436207616, 	 2583691264, 	
		 1509949440, 	 3657433088, 	 973078528, 	 3120562176, 	 2046820352, 	 4194304000, 	 100663296, 	 2248146944, 	 1174405120, 	 3321888768, 	
		 637534208, 	 2785017856, 	 1711276032, 	 3858759680, 	 369098752, 	 2516582400, 	 1442840576, 	 3590324224, 	 905969664, 	 3053453312, 	
		 1979711488, 	 4127195136, 	 234881024, 	 2382364672, 	 1308622848, 	 3456106496, 	 771751936, 	 2919235584, 	 1845493760, 	 3992977408, 	
		 503316480, 	 2650800128, 	 1577058304, 	 3724541952, 	 1040187392, 	 3187671040, 	 2113929216, 	 4261412864
	);
	
	float E1 = fract( float(Index) / NumSamples + float( Random.x ) * (1.0 / 65536.0) );
	float E2 = float( ( ReverseBit32Table[Index] >> 16 ) ^ Random.y ) * (1.0 / 65536.0);
	return vec2( E1, E2 );
#else
	float E1 = fract( float(Index) / NumSamples + float( Random.x ) * (1.0 / 65536.0) );
	float E2 = float( ( ReverseBits32(Index) >> 16 ) ^ Random.y ) * (1.0 / 65536.0);
	return vec2( E1, E2 );
#endif
}
float rcp(float x)
{
	return 1.0 / x;
}

mat3 GetTangentBasis( vec3 TangentZ )
{
	const float Sign = TangentZ.z >= 0 ? 1 : -1;
	const float a = -rcp( Sign + TangentZ.z );
	const float b = TangentZ.x * TangentZ.y * a;
	
	vec3 TangentX = { 1 + Sign * a * pow( TangentZ.x , 2.0), Sign * b, -Sign * TangentZ.x };
	vec3 TangentY = { b,  Sign + a * pow( TangentZ.y , 2.0), -TangentZ.y };

	return mat3( TangentX, TangentY, TangentZ );
}
uvec3 Rand3DPCG16(ivec3 p)
{
	// taking a signed int then reinterpreting as unsigned gives good behavior for negatives
	uvec3 v = uvec3(p);

	// Linear congruential step. These LCG constants are from Numerical Recipies
	// For additional #'s, PCG would do multiple LCG steps and scramble each on output
	// So v here is the RNG state
	v = v * 1664525u + 1013904223u;

	// PCG uses xorshift for the final shuffle, but it is expensive (and cheap
	// versions of xorshift have visible artifacts). Instead, use simple MAD Feistel steps
	//
	// Feistel ciphers divide the state into separate parts (usually by bits)
	// then apply a series of permutation steps one part at a time. The permutations
	// use a reversible operation (usually ^) to part being updated with the result of
	// a permutation function on the other parts and the key.
	//
	// In this case, I'm using v.x, v.y and v.z as the parts, using + instead of ^ for
	// the combination function, and just multiplying the other two parts (no key) for 
	// the permutation function.
	//
	// That gives a simple mad per round.
	v.x += v.y*v.z;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	v.x += v.y*v.z;
	v.y += v.z*v.x;
	v.z += v.x*v.y;

	// only top 16 bits are well shuffled
	return v >> 16u;
}
void main() 
{
	float depth = texture(RT_depth, getScreenCoord()).r;
	vec3 normal = texture(RT_normal, getScreenCoord() ).xyz;
	vec3 baseColor = texture(RT_albedo, getScreenCoord() ).xyz;
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	vec3 viewDir = normalize(worldPos - t_shaderUnifom.TU_camPos.xyz);
	float epsilon = 0.0001;
	vec3 totalColor = vec3(0);
	int NumRays = 4;
	
	uvec2 RandomSeed = Rand3DPCG16(ivec3(gl_FragCoord.x, gl_FragCoord.y, t_shaderUnifom.TU_FrameIndex)).xy;
	//SSGI
	if(depth < 1.0)
	{
		for (int j = 0; j < NumRays; j++)
		{
			//generate the fucking ray dir over hemisphere
			// i : RaySequenceId
			vec2 E = Hammersley16(j, NumRays, RandomSeed);
			// ComputeL
			mat3 TangentBasis = GetTangentBasis(normal);//TBN
			vec3 L;
			L.xy = UniformSampleDiskConcentric( E );
			L.z = sqrt( 1 - dot( L.xy, L.xy ) );
			L = TangentBasis * L; //tangent -> world
			vec3 rayDir = L;//reflect(viewDir, normalize(normal));
			rayDir = normalize(rayDir);
			vec3 stepWorldPos = worldPos;
			float isHit = 0.0;
			depth = LinearizeDepth(depth);
			float stepLength = 1.5;
			float stepDirection = 1.0;
			float NoV = max(0.0, abs(dot(normal, -viewDir)));
			float cosTheta = dot(rayDir, normal);
			float PDF = 1.0;//max(cosTheta / 3.14, 0.027);//minimum 10 degree
			for(int i = 0; i < 8; i++)
			{

				stepWorldPos += rayDir * stepLength * stepDirection;

				vec4 fuckPos = t_shaderUnifom.TU_VP * vec4(stepWorldPos, 1.0);
				fuckPos.xyz /= fuckPos.w;
				vec2 newUV = fuckPos.xy;
				newUV = newUV * 0.5 + vec2(0.5);
				float currDepth = texture(RT_depth, newUV).r;
				currDepth = LinearizeDepth(currDepth);
				fuckPos.z = LinearizeDepth(fuckPos.z);
				vec3 sceneColor = texture(RT_SceneCopy, newUV).rgb;
				if(newUV.x > 1.0 || newUV.x < 0.0 || newUV.y > 1.0 || newUV.y < 0.0 || fuckPos.z >= 1.0 || currDepth >= 1.0)
				{
					break;
				}

				if(fuckPos.z > currDepth) //cross over?
				{
					if(abs(fuckPos.z - currDepth) < epsilon) // good enough?
					{
						float brdf = 1.0 / 3.1416;
						totalColor += (baseColor * brdf * sceneColor) / PDF;
						isHit = 1.0;
						break;
					}
					else //trace back
					{
						stepLength *= 0.5;
						stepDirection = -1.0;
						//放水
						epsilon += 0.00001;
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
		totalColor /= vec3(NumRays);
	}
	out_Color = vec4(totalColor, 1.0 );
}