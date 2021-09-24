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
	vec4 TU_camInfo;
} t_shaderUnifom;



layout(set = 0, binding = 1) uniform sampler2D RT_Bloom0;
layout(set = 0, binding = 2) uniform sampler2D RT_Bloom1;
layout(set = 0, binding = 3) uniform sampler2D RT_Bloom2;

layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}

//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.8;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.45;

void main() 
{
	vec4 bloomColor0 = texture(RT_Bloom0, getScreenCoord());
	vec4 bloomColor1 = texture(RT_Bloom1, getScreenCoord());
	vec4 bloomColor2 = texture(RT_Bloom2, getScreenCoord());
	out_Color = (bloomColor0 + bloomColor1 + bloomColor2) * 0.2;
}