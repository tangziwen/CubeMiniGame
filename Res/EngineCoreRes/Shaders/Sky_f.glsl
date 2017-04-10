#version 420
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
layout(binding = 0)uniform sampler2D TU_Depth;
uniform sampler2D tint;//the color of the sky on the half-sphere where the sun is. (time x height)
uniform sampler2D tint2;//the color of the sky on the opposite half-sphere. (time x height)
uniform sampler2D sun;//sun texture (radius x time)
uniform sampler2D moon;//moon texture (circular)
uniform sampler2D clouds1;//light clouds texture (spherical UV projection)
uniform sampler2D clouds2;//heavy clouds texture (spherical UV projection)
uniform vec2 TU_winSize;
uniform vec3 viewdir;
varying vec2 v_texcoord;
varying vec3 pos;
varying vec3 sun_norm;
varying vec3 star_pos;
uniform float weather;//mixing factor (0.5 to 1.0)
uniform float time;


vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= TU_winSize;
	return tmp;
}
#define E 0.000001
//---------OUT------------
out vec3 color;


#define PI 3.141592
#define iSteps 32
#define jSteps 16

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

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, vec3 kMie, float shRlh, float shMie, float g) {
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


//---------NOISE GENERATION------------
//Noise generation based on a simple hash, to ensure that if a given point on the dome
//(after taking into account the rotation of the sky) is a star, it remains a star all night long
float Hash( float n ){
        return fract( (1.0 + sin(n)) * 415.92653);
}
float Noise3d( vec3 x ){
    float xhash = Hash(round(400*x.x) * 37.0);
    float yhash = Hash(round(400*x.y) * 57.0);
    float zhash = Hash(round(400*x.z) * 67.0);
    return fract(xhash + yhash + zhash);
}

//---------MAIN------------
void main(){
	float depth = texture(TU_Depth, getScreenCoord()).r;
	if(depth < (1.0 - E))
	{
		discard;
		return;
	}
    vec3 pos_norm = normalize(pos);
    float dist = dot(sun_norm,pos_norm);
	
	
	color = atmosphere(
        normalize(pos),           // normalized ray direction
        vec3(0,6372e3,0),               // ray origin
        sun_norm,                        // position of the sun
        22.0,                           // intensity of the sun
        6371e3,                         // radius of the planet in meters
        6471e3,                         // radius of the atmosphere in meters
        vec3(5.8e-6, 13.5e-6, 33.1e-6), // Rayleigh scattering coefficient
        //21e-6,                          // Mie scattering coefficient
		vec3(2.2e-5, 2.2e-5, 2.2e-5),
        10e3,                            // Rayleigh scale height
        1.2e3,                          // Mie scale height
        0.758                           // Mie preferred scattering direction
    );
	return;
	color = 1.0 - exp(-1.0 * color);
	
	float pupil = 1.0f - dot(normalize(-viewdir), sun_norm)/25;
	color = vec3(pupil - exp(-1.0 * (color) ));
	return;
	//Sun
    float radius = length(pos_norm-sun_norm);
    if(radius < 0.05){//We are in the area of the sky which is covered by the sun
        float time = clamp(sun_norm.y,0.01,1.0);
        radius = radius/0.05;
        if(radius < 1.0-0.001){//< we need a small bias to avoid flickering on the border of the texture
            //We read the alpha value from a texture where x = radius and y=height in the sky (~time)
            vec4 sun_color = texture(sun,vec2(radius,time));
            color +=sun_color.rgb;// mix(color,sun_color.rgb,sun_color.a);
        }
    }
	

	
    //We read the tint texture according to the position of the sun and the weather factor
    vec3 color_wo_sun = texture(tint2, vec2((sun_norm.y + 1.0) / 2.0,max(0.01,pos_norm.y))).rgb;
    vec3 color_w_sun = texture(tint, vec2((sun_norm.y + 1.0) / 2.0,max(0.01,pos_norm.y))).rgb;
	color = weather*mix(color_wo_sun,color_w_sun,dist*0.5+0.5);
    //Computing u and v for the clouds textures (spherical projection)
    float u = 0.5 + atan(pos_norm.z,pos_norm.x)/(2*3.14159265);
    float v = - 0.5 + asin(pos_norm.y)/3.14159265;
    //Cloud color
    //color depending on the weather (shade of grey) *  (day or night) ?
    vec3 cloud_color = vec3(min(weather*3.0/2.0,1.0))*(sun_norm.y > 0 ? 0.95 : 0.95+sun_norm.y*1.8);

    //Reading from the clouds maps
    //mixing according to the weather (1.0 -> clouds1 (sunny), 0.5 -> clouds2 (rainy))
    //+ time translation along the u-axis (horizontal) for the clouds movement
    float transparency = mix(texture(clouds2,vec2(u+time,v)).r,texture(clouds1,vec2(u+time,v)).r,(weather-0.5)*2.0);
    // Stars
    if(sun_norm.y<0.1){//Night or dawn
        float threshold = 0.99;
        //We generate a random value between 0 and 1
        float star_intensity = Noise3d(normalize(star_pos));
        //And we apply a threshold to keep only the brightest areas
        if (star_intensity >= threshold){
            //We compute the star intensity
            star_intensity = pow((star_intensity - threshold)/(1.0 - threshold), 6.0)*(-sun_norm.y+0.1);
            color += vec3(star_intensity);
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
        color = mix(color,texture(moon,vec2(x,y)*scale*compensation+vec2(0.5)).rgb,clamp(-sun_norm.y*3,0,1));
    }

    //Final mix
    //mixing with the cloud color allows us to hide things behind clouds (sun, stars, moon)
    color = mix(color,cloud_color,clamp((2-weather)*transparency,0,1));
}
//! [0]

