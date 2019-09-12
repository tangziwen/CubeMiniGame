#version 130

uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_BloomBuffer;
varying vec2 v_texcoord;
void main()
{
	float exposure = 1.5;
	float gamma = 2.2;
	vec4 color = texture2D(TU_colorBuffer, v_texcoord);
	vec4 bloom = texture2D(TU_BloomBuffer, v_texcoord);

	vec3 hdrColor = color.xyz + bloom.xyz;
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));

	gl_FragColor = vec4(result, 1.0);
}
