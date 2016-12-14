#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D TU_topGrass;
uniform sampler2D TU_dirt;
uniform vec4 TU_color;
uniform float TU_roughness;
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec3 v_worldNormal;
varying vec3 v_color;
varying vec3 v_bc;
//! [0]

float edgeFactor(){
	vec3 d = fwidth(v_bc);
	vec3 a3 = smoothstep(vec3(0.0), d*1.5, v_bc);
	return min(min(a3.x, a3.y), a3.z);
}

vec3 powVec(vec3 data, float factor)
{
	return vec3(pow(data.x, factor), pow(data.y, factor), pow(data.z, factor));
}

vec4 triplanarSample(sampler2D sampler)
{
	float plateauSize = 0.5;
	float transitionSpeed = 2.0;
	vec3 blending = abs( normalize(v_worldNormal));
	blending = clamp(blending - plateauSize, 0.0, 1.0);
	blending = powVec(blending, transitionSpeed);
	blending /= dot(blending, vec3(1.0, 1.0, 1.0));
	float scale = 0.5;
	vec4 xaxis = texture2D( sampler, v_worldPos.yz * scale);
	vec4 yaxis = texture2D( sampler, v_worldPos.xz * scale);
	vec4 zaxis = texture2D( sampler, v_worldPos.xy * scale);
	return xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
}

vec4 outLineFilter(vec4 color)
{
	if(edgeFactor() < 0.3)
	{
		return vec4(1.0,0.0,0.0,1.0);
	}
	else
	{
		return color;
	}
}
void main()
{
    // Set fragment color from texture
	

	//triplanar texture mapping
	
	vec4 diffuseColor = 0.0;
	diffuseColor += triplanarSample(TU_topGrass);
	vec4 resultColor = diffuseColor * vec4(v_color.rgb, 1.0) * TU_color;

    gl_FragData[0] = resultColor;//outLineFilter(resultColor);
	gl_FragData[1] = vec4(v_position,1.0);
	gl_FragData[2] = vec4(normalize(v_normal),1.0);		
	gl_FragData[3] = vec4(TU_roughness,0.0,0.0,1.0);
}
//! [0]