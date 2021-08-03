
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
	vec4 TU_camInfo;
	vec4 TU_ProjInfo;
	vec4 TU_RadiusInfo;// radius, radius*radius, radiusToscreen
} t_shaderUnifom;
layout(set = 0, binding = 1) uniform sampler2D RT_oldTAA;
//layout(set = 0, binding = 2) uniform sampler2D RT_CurrScene;


vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}
void main() 
{
	vec4 oldTaa = texture(RT_oldTAA, getScreenCoord());
	//vec4 currScene = texture(RT_CurrScene, getScreenCoord());
	out_Color = oldTaa * vec4(1, 0, 0 ,1);//vec4( (oldTaa.xyz + currScene.xyz) * 0.5, 1.0);
}
