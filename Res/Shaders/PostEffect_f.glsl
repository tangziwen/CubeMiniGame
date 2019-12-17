

uniform float F0 = 0.8;
uniform float K = 0.7;
uniform float Gamma = 1.4;

uniform sampler2D TU_colorBuffer;
uniform vec2 TU_winSize;

varying vec2 v_texcoord;
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= TU_winSize;
	return tmp;
}

float LinearizeDepth(float near, float far, float depth) 
{
    // 计算ndc坐标 这里默认glDepthRange(0,1)
    float Zndc = depth * 2.0 - 1.0; 
    // 这里分母进行了反转
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near)); 
    return (Zeye - near)/ ( far - near);
}

void main()
{
	vec4 color = texture2D(TU_colorBuffer, v_texcoord);
	float gray = color.x * 0.3 + color.y * 0.6 + color.z * 0.1;
	//gl_FragColor = vec4(gray, gray, gray,1.0);
	gl_FragColor = color;
}
