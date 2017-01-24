#version 130
struct DirectionalLight
{
    vec3 color;
    vec3 direction;
};

struct AmbientLight
{
    vec3 color;
    float intensity;
};

uniform float F0 = 0.8;
uniform float K = 0.7;
uniform float Gamma = 1.4;

uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_posBuffer;
uniform sampler2D TU_normalBuffer;
uniform sampler2D TU_GBUFFER4;
uniform sampler2D TU_Depth;
uniform vec2 TU_winSize;

uniform DirectionalLight gDirectionalLight;
uniform AmbientLight gAmbientLight;

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

float getDist(float near, float far)
{
	float nonLinearDepth = texture2D(TU_Depth, getScreenCoord()).r;
	float linearDepth = LinearizeDepth(near, far, nonLinearDepth);
	return near + (far - near) * linearDepth;
}

float getFogFactor(float start, float end, float dist)
{
	return (dist - start)/ (end - start);
}


float getFogFactorEXP(float density, float dist)
{
	return 1.0 - 1.0/pow(2.71828,pow(dist * density, 2.0));
}
vec3 calculateLightPBR(vec3 normal, vec3 lightDir, vec3 lightColor,vec3 viewPos,float Roughness)
{
	lightDir = -lightDir;
	vec3 diffuseColor,ambientColor;
	float specularIntensity =0.0;
	float irradiance = max(0.0,dot(normal, lightDir));
	if(irradiance > 0.0)
	{
	
		vec3 viewDir = normalize(-viewPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		
		float NdotL = irradiance;
		float NdotH = max(dot(normal, halfDir), 0.0);
		float NdotV = max(dot(normal, viewDir), 0.0);
		float VdotH = max(dot(viewDir, halfDir), 0.0);
		float mSquared = Roughness * Roughness;

		// geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));		

		// roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);

		// fresnel
        // Schlick approximation
        float fresnel = pow(1.0 - VdotH, 5.0);
        fresnel *= (1.0 - F0);
        fresnel += F0;
        
	specularIntensity = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
	specularIntensity = max(specularIntensity, 0.0);
	}
	vec3 colorLinear = 1.5 * lightColor * irradiance * (K + specularIntensity * (1.0 - K)) + gAmbientLight.color * gAmbientLight.intensity;
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / Gamma));
	return colorLinear;
}


vec3 calculateLightLambert(vec3 normal, vec3 lightDir, vec3 lightColor,vec3 viewPos,float Roughness)
{
	lightDir = -lightDir;
	vec3 diffuseColor,ambientColor;
	float specularIntensity =0.0;
	float irradiance = max(0.0,dot(normal, lightDir));
	return lightColor * irradiance + gAmbientLight.color * gAmbientLight.intensity;
}


vec3 calculateLightBlinnPhong(vec3 normal, vec3 lightDir, vec3 lightColor,vec3 viewPos,float Roughness)
{
	lightDir = -lightDir;
	vec3 diffuseColor,ambientColor;
	float specularIntensity =0.0;
	float irradiance = max(0.0,dot(normal, lightDir));
	if(irradiance > 0.0)
	{
		vec3 viewDir = normalize(-viewPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		specularIntensity = pow( max( 0, dot(halfDir, normal)), 2.0);
	}
	return lightColor * specularIntensity;
	return lightColor * irradiance + gAmbientLight.color * gAmbientLight.intensity;
}

void main()
{
	vec4 Data1 = texture2D(TU_colorBuffer, v_texcoord);
	vec3 color = Data1.xyz;
	float roughness = texture2D(TU_GBUFFER4, v_texcoord).r;
	vec3 pos = texture2D(TU_posBuffer, v_texcoord).xyz;
	vec3 normal = normalize(texture2D(TU_normalBuffer, v_texcoord).xyz);
	
	//gl_FragColor = vec4(vec3(1.0,1.0,1.0) * calculateLightPBR(normal, gDirectionalLight.direction, gDirectionalLight.color, pos, roughness),Data1.a);/
	
	vec4 finalColor = vec4(color * calculateLightPBR(normal, gDirectionalLight.direction, gDirectionalLight.color, pos, roughness),Data1.a);
	float fogFactor = getFogFactor(50.0, 250.0, -1 * pos.z);
	gl_FragColor = finalColor * (1.0 - fogFactor) + fogFactor * vec4(176.0 / 255.0 , 171.0 / 255.0, 203.0 / 255.0, 1.0);
}
