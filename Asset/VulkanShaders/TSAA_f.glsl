
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
	mat4 TU_ClipToHistory;
	vec4 TU_HistoryInfo;// previous near/far, history valid, unused
	vec4 TU_camInfo;
	vec4 TU_ProjInfo;
	vec4 TU_RadiusInfo;// radius, radius*radius, radiusToscreen
	vec4 TU_TSAAResolveParams;
	vec4 TU_TSAARejectionParams;
	int TU_TSAADebugMode;
} t_shaderUnifom;
layout(set = 0, binding = 1) uniform sampler2D RT_oldTAA;
layout(set = 0, binding = 2) uniform sampler2D RT_CurrScene;
// Engine Vulkan depth is 0 near and 1 far; clear/background depth is 1, and LESS
// makes smaller valid depth closer. The depth sampler may be linear/repeat, so
// edge-safe reprojection uses the integer fetch helpers below.
layout(set = 0, binding = 3) uniform sampler2D RT_depth;
// The TSAA depth attachment stores positive view distance divided by that frame's far plane.
layout(set = 0, binding = 4) uniform sampler2D RT_historyDepth;

struct DepthNeighborhood
{
	float closestDepth;
	float minDepth;
	float maxDepth;
	bool hasValidDepth;
};

ivec2 getFramebufferSize()
{
	return ivec2(t_shaderUnifom.TU_winSize);
}

ivec2 clampPixelCoord(ivec2 pixel)
{
	ivec2 maxPixel = getFramebufferSize() - ivec2(1);
	return clamp(pixel, ivec2(0), maxPixel);
}

float fetchDepthPixel(ivec2 pixel)
{
	return texelFetch(RT_depth, clampPixelCoord(pixel), 0).r;
}

bool isValidDepth(float depth)
{
	return depth < 0.999999;
}

DepthNeighborhood selectReprojectionDepth(vec2 currTC)
{
	DepthNeighborhood result;
	result.closestDepth = 1.0;
	result.minDepth = 1.0;
	result.maxDepth = 0.0;
	result.hasValidDepth = false;

	ivec2 basePixel = clampPixelCoord(ivec2(floor(currTC * t_shaderUnifom.TU_winSize)));
	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			float sampleDepth = fetchDepthPixel(basePixel + ivec2(x, y));
			if(!isValidDepth(sampleDepth))
			{
				continue;
			}

			if(!result.hasValidDepth)
			{
				result.minDepth = sampleDepth;
				result.maxDepth = sampleDepth;
				result.closestDepth = sampleDepth;
				result.hasValidDepth = true;
			}
			else
			{
				result.minDepth = min(result.minDepth, sampleDepth);
				result.maxDepth = max(result.maxDepth, sampleDepth);
				result.closestDepth = min(result.closestDepth, sampleDepth);
			}
		}
	}
	return result;
}

float getViewDepth(float depthValue, float nearPlane, float farPlane)
{
	return nearPlane * farPlane / max(farPlane - depthValue * (farPlane - nearPlane), 0.000001);
}

vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;
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

void computeNeighborhoodAABB(vec2 centerUV, vec2 texelSize, out vec3 cMin, out vec3 cMax)
{
	const float gamma = 1.25;
	vec3 m1 = vec3(0.0);
	vec3 m2 = vec3(0.0);
	vec3 hardMin = vec3(1e5);
	vec3 hardMax = vec3(-1e5);

	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			vec2 sampleTC = clamp(centerUV + vec2(float(x), float(y)) * texelSize, 0.5 * texelSize, vec2(1.0) - 0.5 * texelSize);
			vec3 sampleYCoCg = RGB2YCoCg(texture(RT_CurrScene, sampleTC).rgb);
			m1 += sampleYCoCg;
			m2 += sampleYCoCg * sampleYCoCg;
			hardMin = min(hardMin, sampleYCoCg);
			hardMax = max(hardMax, sampleYCoCg);
		}
	}

	m1 /= 9.0;
	m2 /= 9.0;
	vec3 sigma = sqrt(max(m2 - m1 * m1, vec3(0.0)));

	cMin = max(hardMin, m1 - gamma * sigma);
	cMax = min(hardMax, m1 + gamma * sigma);
}

vec3 clipHistoryToAabb(vec3 historyYCoCg, vec3 cMin, vec3 cMax)
{
	vec3 center = 0.5 * (cMin + cMax);
	vec3 extent = 0.5 * (cMax - cMin) + vec3(0.0001);
	vec3 offset = historyYCoCg - center;
	vec3 unit = offset / extent;
	float maxUnit = max(abs(unit.x), max(abs(unit.y), abs(unit.z)));
	if(maxUnit > 1.0)
	{
		return center + offset / maxUnit;
	}
	return historyYCoCg;
}

void main() 
{
	float baseHistoryWeight = t_shaderUnifom.TU_TSAAResolveParams.x;
	float minHistoryWeight = t_shaderUnifom.TU_TSAAResolveParams.y;
	float edgeDepthRangeThreshold = t_shaderUnifom.TU_TSAAResolveParams.z;
	float motionWeightScale = t_shaderUnifom.TU_TSAAResolveParams.w;
	float lumaRejectThreshold = t_shaderUnifom.TU_TSAARejectionParams.x;
	float edgeHistoryWeight = t_shaderUnifom.TU_TSAARejectionParams.y;
	float relativeDepthTolerance = t_shaderUnifom.TU_TSAARejectionParams.z;
	float absoluteDepthTolerance = t_shaderUnifom.TU_TSAARejectionParams.w;
	int debugMode = t_shaderUnifom.TU_TSAADebugMode;

	vec2 tc = getScreenCoord();
	vec2 one_over_size = vec2(1.0) / t_shaderUnifom.TU_winSize;
	// Current color/depth use jittered UV; the history transform consumes unjittered clip coordinates.
	vec2 currTC = clamp(tc + t_shaderUnifom.TU_jitterUV, 0.5 * one_over_size, vec2(1.0) - 0.5 * one_over_size);
	vec4 currScene = texture(RT_CurrScene, currTC);
	float centerDepth = fetchDepthPixel(ivec2(floor(currTC * t_shaderUnifom.TU_winSize)));
	float viewDepth = getViewDepth(centerDepth, t_shaderUnifom.TU_camInfo.x, t_shaderUnifom.TU_camInfo.y);
	// Zero marks background; all written depths stay below the clear value for the LESS depth test.
	gl_FragDepth = isValidDepth(centerDepth) ? clamp(viewDepth / t_shaderUnifom.TU_camInfo.y, 0.0000001, 0.999999) : 0.0;
	if(t_shaderUnifom.TU_HistoryInfo.z < 0.5 || !isValidDepth(centerDepth))
	{
		out_Color = vec4(currScene.rgb, 1.0);
		return;
	}

	vec3 cMin, cMax;
	computeNeighborhoodAABB(currTC, one_over_size, cMin, cMax);

	DepthNeighborhood depthNeighborhood = selectReprojectionDepth(currTC);
	if(!depthNeighborhood.hasValidDepth)
	{
		out_Color = vec4(currScene.xyz, 1.0);
		return;
	}

	float depth = depthNeighborhood.closestDepth;
	float depthRange = depthNeighborhood.maxDepth - depthNeighborhood.minDepth;
	vec4 lastNDC = t_shaderUnifom.TU_ClipToHistory * vec4(tc * 2.0 - 1.0, depth, 1.0);
	if(lastNDC.w <= 0.0)
	{
		out_Color = vec4(currScene.xyz, 1.0);
		return;
	}
	lastNDC /= lastNDC.w;
	
	vec2 lastUV = vec2(lastNDC.x * 0.5 + 0.5, lastNDC.y * 0.5 + 0.5);
	if(lastUV.x < 0.0 || lastUV.x > 1.0 || lastUV.y < 0.0 || lastUV.y > 1.0 || lastNDC.z < 0.0 || lastNDC.z >= 1.0)
	{
		out_Color = vec4(currScene.xyz, 1.0);
		return;
	}

	vec2 historyUV = clamp(lastUV, 0.5 * one_over_size, vec2(1.0) - 0.5 * one_over_size);
	vec4 centerLastClip = t_shaderUnifom.TU_ClipToHistory * vec4(tc * 2.0 - 1.0, centerDepth, 1.0);
	if(centerLastClip.w <= 0.0 || centerLastClip.z < 0.0 || centerLastClip.z >= centerLastClip.w)
	{
		out_Color = vec4(currScene.rgb, 1.0);
		return;
	}
	float expectedHistoryDepth = getViewDepth(centerLastClip.z / centerLastClip.w, t_shaderUnifom.TU_HistoryInfo.x, t_shaderUnifom.TU_HistoryInfo.y);
	ivec2 historyPixel = clampPixelCoord(ivec2(floor(historyUV * t_shaderUnifom.TU_winSize)));
	float storedHistoryDepth = texelFetch(RT_historyDepth, historyPixel, 0).r;
	float historyViewDepth = storedHistoryDepth * t_shaderUnifom.TU_HistoryInfo.y;
	float depthTolerance = max(absoluteDepthTolerance, relativeDepthTolerance * expectedHistoryDepth);
	bool historyRejected = storedHistoryDepth <= 0.0 || storedHistoryDepth >= 1.0
		|| abs(historyViewDepth - expectedHistoryDepth) > depthTolerance;
	vec3 historyYCoCg = RGB2YCoCg(texture(RT_oldTAA, historyUV).rgb);
	vec3 clippedHistoryYCoCg = clipHistoryToAabb(historyYCoCg, cMin, cMax);
	vec3 clippedHistoryRgb = YCoCg2RGB(clippedHistoryYCoCg);

	float historyWeight = baseHistoryWeight;
	historyWeight = mix(historyWeight, edgeHistoryWeight, smoothstep(edgeDepthRangeThreshold, edgeDepthRangeThreshold * 3.0, depthRange));
	float motionPixels = length((lastUV - tc) * t_shaderUnifom.TU_winSize);
	float motionFactor = clamp(motionPixels * motionWeightScale, 0.0, 1.0);
	historyWeight = mix(historyWeight, minHistoryWeight, motionFactor);

	float currY = RGB2YCoCg(currScene.rgb).x;
	float historyY = historyYCoCg.x;
	float relativeLumaDifference = abs(historyY - currY) / max(max(abs(historyY), abs(currY)), 0.1);
	float lumaRejection = smoothstep(lumaRejectThreshold * 0.5, lumaRejectThreshold, relativeLumaDifference);
	historyWeight *= 1.0 - lumaRejection;
	historyRejected = historyRejected || lumaRejection >= 1.0;
	historyWeight = historyRejected ? 0.0 : clamp(historyWeight, 0.0, baseHistoryWeight);

	vec3 finalColor = mix(currScene.rgb, clippedHistoryRgb, historyWeight);
	if(debugMode == 0)
	{
		out_Color = vec4(finalColor, 1.0);
	}
	else if(debugMode == 1)
	{
		out_Color = vec4(vec3(historyWeight), 1.0);
	}
	else if(debugMode == 2)
	{
		float edgeSignal = clamp(depthRange / edgeDepthRangeThreshold, 0.0, 1.0);
		vec3 debugColor = vec3(0.0, historyWeight, edgeSignal);
		if(historyRejected)
		{
			debugColor = vec3(1.0, 0.0, edgeSignal);
		}
		out_Color = vec4(debugColor, 1.0);
	}
	else if(debugMode == 3)
	{
		out_Color = vec4(vec3(1.0 - depth), 1.0);
	}
	else
	{
		out_Color = vec4(finalColor, 1.0);
	}
}
