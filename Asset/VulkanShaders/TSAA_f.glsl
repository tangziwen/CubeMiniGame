
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



vec4 getWorldPosFromDepth(float depthValue)
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = v_texcoord * 2.0 - 1.0;
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
void main() 
{
	vec2 tc = getScreenCoord();
	vec2 one_over_size = vec2(1.0) / t_shaderUnifom.TU_winSize;
	vec4 currScene = texture(RT_CurrScene, tc);
	vec4 c0 = texture(RT_CurrScene, tc + vec2(one_over_size.x, 0));
	vec4 c1 = texture(RT_CurrScene, tc + vec2(-one_over_size.x, 0));
	vec4 c2 = texture(RT_CurrScene, tc + vec2(0, one_over_size.y));
	vec4 c3 = texture(RT_CurrScene, tc + vec2(0, -one_over_size.y));
	vec4 cMin = min(min(min(c0,c1), min(c2,c3)), currScene);
	vec4 cMax = max(max(max(c0,c1), max(c2,c3)), currScene);

	float depth = texture(RT_depth, getScreenCoord()).r;
	vec3 worldPos = getWorldPosFromDepth(depth).xyz;
	vec2 lastUV;
	vec4 lastNDC = t_shaderUnifom.TU_LastVP * vec4(worldPos, 1.0);
	lastNDC /= lastNDC.w;
	lastUV = vec2(lastNDC.x * 0.5 + 0.5, lastNDC.y * 0.5 + 0.5);
	vec4 oldTaa = texture(RT_oldTAA, lastUV);
	if(lastUV.x < 0.0 || lastUV.x > 1.0 || lastUV.y < 0.0 || lastUV.y > 1.0)
	{
		out_Color = vec4((currScene.xyz ), 1.0);
	}
	else
	{
		oldTaa.rgb = clamp(oldTaa.rgb, cMin.rgb, cMax.rgb);
		out_Color = vec4((oldTaa.xyz * 0.9 + currScene.xyz * 0.1), 1.0);
	}
}
