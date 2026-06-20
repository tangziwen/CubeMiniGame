
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
layout(set = 0, binding = 0) uniform UniformBufferObjectMat 
{
	vec4 TU_color;
	float fog_near;
	float fog_far;
	float fog_height_min;
	float fog_height_max;
	vec3 fog_color;
	vec2 TU_winSize;
	vec2 TU_jitterUV;
	vec3 TU_camPos;
	mat4 TU_viewProjectInverted;
	mat4 TU_LastVP;
	vec4 TU_camInfo;
	vec4 TU_ProjInfo;
	vec4 TU_RadiusInfo;// radius, radius*radius, radiusToscreen
} t_shaderUnifom;
layout(set = 0, binding = 1) uniform sampler2D RT_oldTAA;
layout(set = 0, binding = 2) uniform sampler2D RT_CurrScene;
layout(set = 0, binding = 3) uniform sampler2D RT_depth;



vec4 getWorldPosFromDepth(float depthValue, vec2 uv)
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = uv * 2.0 - 1.0;
  clipSpaceLocation.z = depthValue;
  clipSpaceLocation.w = 1.0;
  vec4 homogenousLocation = t_shaderUnifom.TU_viewProjectInverted * clipSpaceLocation;
  return vec4(homogenousLocation.xyz / homogenousLocation.w, 1.0);
}

vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}

float Luma4(vec3 Color)
{
	return (Color.g * 2.0) + (Color.r + Color.b);
}


vec3 RGB2YCoCg(vec3 rgb)
{
	float R  = rgb.r; float G = rgb.g; float B = rgb.b;
	float T = 0.25*R + 0.25*B;
	float Y  = 0.5*G + T;
	float Cg = 0.5*G - T;
	float Co = 0.5*(R-B);
	return vec3(Y, Cg, Co);
}

vec3 YCoCg2RGB(vec3 YCoCg)
{
	float Y  = YCoCg.x; float Cg = YCoCg.y; float Co = YCoCg.z;
	float t = Y - Cg;
	float R = t + Co;
	float G = Y + Cg;
	float B = t - Co;

	return vec3(R, G, B);
}

void main() 
{
	vec2 tc = getScreenCoord();
	vec2 one_over_size = vec2(1.0) / t_shaderUnifom.TU_winSize;
	vec2 currTC = clamp(tc + t_shaderUnifom.TU_jitterUV, vec2(0.0), vec2(1.0));
	vec4 currScene = texture(RT_CurrScene, currTC);
	vec3 currSceneYCoCg = RGB2YCoCg(currScene.rgb);
	vec3 cMin = currSceneYCoCg;
	vec3 cMax = currSceneYCoCg;
	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			vec2 sampleTC = clamp(currTC + vec2(float(x), float(y)) * one_over_size, vec2(0.0), vec2(1.0));
			vec3 sampleYCoCg = RGB2YCoCg(texture(RT_CurrScene, sampleTC).rgb);
			cMin = min(cMin, sampleYCoCg);
			cMax = max(cMax, sampleYCoCg);
		}
	}

	float depth = texture(RT_depth, currTC).r;
	if(depth >= 1.0)
	{
		out_Color = vec4(currScene.xyz, 1.0);
		return;
	}

	vec3 worldPos = getWorldPosFromDepth(depth, tc).xyz;
	vec2 lastUV;
	vec4 lastNDC = t_shaderUnifom.TU_LastVP * vec4(worldPos, 1.0);
	if(lastNDC.w <= 0.0)
	{
		out_Color = vec4(currScene.xyz, 1.0);
		return;
	}
	lastNDC /= lastNDC.w;
	
	lastUV = vec2(lastNDC.x * 0.5 + 0.5, lastNDC.y * 0.5 + 0.5);
	if(lastUV.x < 0.0 || lastUV.x > 1.0 || lastUV.y < 0.0 || lastUV.y > 1.0)
	{
		out_Color = vec4((currScene.xyz ), 1.0);
	}
	else
	{
		vec4 oldTaa = texture(RT_oldTAA, lastUV);
		//oldTaa.rgb = clamp(oldTaa.rgb, cMin.rgb, cMax.rgb);
		oldTaa.rgb = RGB2YCoCg(oldTaa.rgb);
		oldTaa.rgb = clamp(oldTaa.rgb, cMin, cMax);
		oldTaa.rgb = YCoCg2RGB(oldTaa.rgb);
		out_Color = vec4((oldTaa.xyz * 0.90 + currScene.xyz * 0.1), 1.0);
	}
}
