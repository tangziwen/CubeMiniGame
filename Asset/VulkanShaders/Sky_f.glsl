layout(set = 0, binding = 1) uniform sampler2D DepthMap;

layout(set = 0, binding = 0) uniform UniformBufferObjectMat 
{
	vec3 TU_sunDirection;
	vec2 TU_winSize;
	float weather;
} t_shaderUnifom;


layout(set = 0, binding = 2) uniform sampler2D tint;//the color of the sky on the half-sphere where the sun is. (time x height)
layout(set = 0, binding = 3) uniform sampler2D tint2;//the color of the sky on the opposite half-sphere. (time x height)
layout(set = 0, binding = 4) uniform sampler2D sun;//sun texture (radius x time)
layout(set = 0, binding = 5) uniform sampler2D moon;//moon texture (circular)
layout(set = 0, binding = 6) uniform sampler2D clouds1;//light clouds texture (spherical UV projection)
layout(set = 0, binding = 7) uniform sampler2D clouds2;//heavy clouds texture (spherical UV projection)


layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
layout(location = 2) in vec3 v_position;

layout(location = 0) out vec4 out_Color;


#define E 0.000001
#define PI 3.141592
#define iSteps 16
#define jSteps 8

vec4 decodeGamma(vec4 col)
{
    return vec4(pow(col.xyz, vec3(2.2)), col.w);
}
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= t_shaderUnifom.TU_winSize;//vec2(640, 480);
	return tmp;
}

vec2 rsi(vec3 r0, vec3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5,-1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0,0,0);
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(iSteps);

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {

            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

void main() 
{
	float depth = texture(DepthMap, getScreenCoord()).r;
	float dFactor = step(depth, 0.999995);
	float time = 0.0;
    vec3 skyColor = atmosphere(
        v_position,           // normalized ray direction
        vec3(0,6372e3,0),               // ray origin
        normalize(-t_shaderUnifom.TU_sunDirection),                        // position of the sun
        50,                           // intensity of the sun
        6371e3,                         // radius of the planet in meters
        6471e3,                         // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
        8e3,                            // Rayleigh scale height
        1.2e3,                          // Mie scale height
        0.758                           // Mie preferred scattering direction
    );
	skyColor = 1.0 - exp(-1.0 * skyColor);
	
	vec3 pos_norm = normalize(v_position); 
	vec3 sun_norm = normalize(-t_shaderUnifom.TU_sunDirection);
    //Computing u and v for the clouds textures (spherical projection)
    float u = 0.5 + atan(pos_norm.z,pos_norm.x)/(2*3.14159265);
    float v = -0.5 + asin(pos_norm.y)/3.14159265;
	
    //Cloud color
    //color depending on the weather (shade of grey) *  (day or night) ?
    vec3 cloud_color = vec3(min(t_shaderUnifom.weather*3.0/2.0,1.0))*(sun_norm.y > 0 ? 0.95 : 0.95+sun_norm.y*1.8);
	
    //Reading from the clouds maps
    //mixing according to the weather (1.0 -> clouds1 (sunny), 0.5 -> clouds2 (rainy))
    //+ time translation along the u-axis (horizontal) for the clouds movement
    float transparency = mix(decodeGamma(texture(clouds2,vec2(u+time,v))).r,decodeGamma(texture(clouds1,vec2(u+time,v))).r,(t_shaderUnifom.weather-0.5)*2.0);
    
	/*
	// Stars
    if(sun_norm.y<0.1){//Night or dawn
        float threshold = 0.99;
        //We generate a random value between 0 and 1
        float star_intensity = Noise3d(normalize(star_pos));
        //And we apply a threshold to keep only the brightest areas
        if (star_intensity >= threshold){
            //We compute the star intensity
            star_intensity = pow((star_intensity - threshold)/(1.0 - threshold), 6.0)*(-sun_norm.y+0.1);
            skyColor += vec3(star_intensity);
        }

    }
	*/

    float radius = length(pos_norm-sun_norm);
    if(radius < 0.05){//We are in the area of the sky which is covered by the sun
        float time = clamp(sun_norm.y,0.01,1.0);
        radius = radius/0.05;
        if(radius < 1.0-0.001){//< we need a small bias to avoid flickering on the border of the texture
            //We read the alpha value from a texture where x = radius and y=height in the sky (~time)
            vec4 sun_color = decodeGamma(texture(sun,vec2(radius,time)));
            skyColor = mix(skyColor,sun_color.rgb * 6.0,sun_color.a);
        }
    }
    //Moon
    float radius_moon = length(pos_norm+sun_norm);//the moon is at position -sun_pos
    if(radius_moon < 0.03){//We are in the area of the sky which is covered by the moon
        //We define a local plane tangent to the skydome at -sun_norm
        //We work in model space (everything normalized)
        vec3 n1 = normalize(cross(-sun_norm,vec3(0,1,0)));
        vec3 n2 = normalize(cross(-sun_norm,n1));
        //We project pos_norm on this plane
        float x = dot(pos_norm,n1);
        float y = dot(pos_norm,n2);
        //x,y are two sine, ranging approx from 0 to sqrt(2)*0.03. We scale them to [-1,1], then we will translate to [0,1]
        float scale = 23.57*0.5;
        //we need a compensation term because we made projection on the plane and not on the real sphere + other approximations.
        float compensation = 1.4;
        //And we read in the texture of the moon. The projection we did previously allows us to have an undeformed moon
        //(for the sun we didn't care as there are no details on it)
        skyColor = mix(skyColor,decodeGamma(texture(moon,vec2(x,y)*scale*compensation+vec2(0.5))).rgb,clamp(-sun_norm.y*3,0,1));
    }
    //Final mix
    //mixing with the cloud color allows us to hide things behind clouds (sun, stars, moon)
	skyColor = mix(skyColor,cloud_color,clamp((2-t_shaderUnifom.weather)*transparency,0,1));
	out_Color = vec4(skyColor, mix(1, 0, dFactor));
}
