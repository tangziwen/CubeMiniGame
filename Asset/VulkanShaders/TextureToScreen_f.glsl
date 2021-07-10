#pragma vscode_glsllint_stage : frag 
#define PI 3.141592654
layout(set = 0, binding = 1) uniform sampler2D RenderTarget1;
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;

vec3 ACES_Knarkowicz( vec3 x )
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float s = 0.14;
	return (x*(a*x+b))/(x*(c*x+d)+s);
}

vec3 uchimura(vec3 x, float P, float a, float m, float l, float c, float b) {
  float l0 = ((P - m) * l) / a;
  float L0 = m - m / a;
  float L1 = m + (1.0 - m) / a;
  float S0 = m + l0;
  float S1 = m + a * l0;
  float C2 = (a * P) / (P - S1);
  float CP = -C2 / P;

  vec3 w0 = vec3(1.0 - smoothstep(0.0, m, x));
  vec3 w2 = vec3(step(m + l0, x));
  vec3 w1 = vec3(1.0 - w0 - w2);

  vec3 T = vec3(m * pow(x / m, vec3(c)) + b);
  vec3 S = vec3(P - (P - S1) * exp(CP * (x - S0)));
  vec3 L = vec3(m + a * (x - m));

  return T * w0 + L * w1 + S * w2;
}

vec3 uchimura(vec3 x) {
  const float P = 1.0;  // max display brightness
  const float a = 1.0;  // contrast
  const float m = 0.22; // linear section start
  const float l = 0.4;  // linear section length
  const float c = 1.33; // black
  const float b = 0.0;  // pedestal

  return uchimura(x, P, a, m, l, c, b);
}




const mat3 AP0_2_XYZ_MAT = mat3
(
	0.9525523959, 0.0000000000, 0.0000936786,
	0.3439664498, 0.7281660966,-0.0721325464,
	0.0000000000, 0.0000000000, 1.0088251844
);

 const mat3 XYZ_2_AP0_MAT =mat3
(

	 1.0498110175, 0.0000000000,-0.0000974845,
	-0.4959030231, 1.3733130458, 0.0982400361,
	 0.0000000000, 0.0000000000, 0.9912520182
);

 const mat3 AP1_2_XYZ_MAT =mat3
(
	 0.6624541811, 0.1340042065, 0.1561876870,
	 0.2722287168, 0.6740817658, 0.0536895174,
	-0.0055746495, 0.0040607335, 1.0103391003
);

 const mat3 XYZ_2_AP1_MAT =mat3
(
	 1.6410233797, -0.3248032942, -0.2364246952,
	-0.6636628587,  1.6153315917,  0.0167563477,
	 0.0117218943, -0.0082844420,  0.9883948585
);

 const mat3 AP0_2_AP1_MAT =mat3
(//mul( AP0_2_XYZ_MAT, XYZ_2_AP1_MAT );
	 1.4514393161, -0.2365107469, -0.2149285693,
	-0.0765537734,  1.1762296998, -0.0996759264,
	 0.0083161484, -0.0060324498,  0.9977163014
);

 const mat3 AP1_2_AP0_MAT  =mat3
(//mul( AP1_2_XYZ_MAT, XYZ_2_AP0_MAT );
	 0.6954522414,  0.1406786965,  0.1638690622,
	 0.0447945634,  0.8596711185,  0.0955343182,
	-0.0055258826,  0.0040252103,  1.0015006723
);

 const vec3 AP1_RGB2Y =vec3
(
	0.2722287168, //AP1_2_XYZ_MAT[0][1],
	0.6740817658, //AP1_2_XYZ_MAT[1][1],
	0.0536895174 //AP1_2_XYZ_MAT[2][1]
);

// REC 709 primaries
 const mat3 XYZ_2_sRGB_MAT =mat3
(
	 3.2409699419, -1.5373831776, -0.4986107603,
	-0.9692436363,  1.8759675015,  0.0415550574,
	 0.0556300797, -0.2039769589,  1.0569715142
);

 const mat3 sRGB_2_XYZ_MAT =mat3
(
	0.4124564, 0.3575761, 0.1804375,
	0.2126729, 0.7151522, 0.0721750,
	0.0193339, 0.1191920, 0.9503041
);

// REC 2020 primaries
 const mat3 XYZ_2_Rec2020_MAT =mat3
(
	 1.7166084, -0.3556621, -0.2533601,
	-0.6666829,  1.6164776,  0.0157685,
	 0.0176422, -0.0427763,  0.94222867	
);

 const mat3 Rec2020_2_XYZ_MAT =mat3
(
	0.6369736, 0.1446172, 0.1688585,
	0.2627066, 0.6779996, 0.0592938,
	0.0000000, 0.0280728, 1.0608437
);

// P3, D65 primaries
 const mat3 XYZ_2_P3D65_MAT =mat3
(
	 2.4933963, -0.9313459, -0.4026945,
	-0.8294868,  1.7626597,  0.0236246,
	 0.0358507, -0.0761827,  0.9570140
);

 const mat3 P3D65_2_XYZ_MAT =mat3
(
	0.4865906, 0.2656683, 0.1981905,
	0.2289838, 0.6917402, 0.0792762,
	0.0000000, 0.0451135, 1.0438031
);

// Bradford chromatic adaptation transforms between ACES white point (D60) and sRGB white point (D65)
 const mat3 D65_2_D60_CAT =mat3
(
	 1.01303,    0.00610531, -0.014971,
	 0.00769823, 0.998165,   -0.00503203,
	-0.00284131, 0.00468516,  0.924507
);

 const mat3 D60_2_D65_CAT =mat3
(
	 0.987224,   -0.00611327, 0.0159533,
	-0.00759836,  1.00186,    0.00533002,
	 0.00307257, -0.00509595, 1.08168
);

const float HALF_MAX = 65504.0;

#define log10(x)  log((x)) / log(10.0)
float Square( float x )
{
	return x*x;
}

float rgb_2_saturation( vec3 rgb )
{
	float minrgb = min( min(rgb.r, rgb.g ), rgb.b );
	float maxrgb = max( max(rgb.r, rgb.g ), rgb.b );
	return ( max( maxrgb, 1e-10 ) - max( minrgb, 1e-10 ) ) / max( maxrgb, 1e-2 );
}

float glow_fwd( float ycIn, float glowGainIn, float glowMid)
{
   float glowGainOut;

   if (ycIn <= 2./3. * glowMid) {
	 glowGainOut = glowGainIn;
   } else if ( ycIn >= 2 * glowMid) {
	 glowGainOut = 0;
   } else {
	 glowGainOut = glowGainIn * (glowMid / ycIn - 0.5);
   }

   return glowGainOut;
}

float center_hue( float hue, float centerH)
{
	float hueCentered = hue - centerH;
	if (hueCentered < -180.)
		hueCentered += 360;
	else if (hueCentered > 180.)
		hueCentered -= 360;
	return hueCentered;
}

// Transformations from RGB to other color representations
float rgb_2_hue( vec3 rgb ) 
{
	// Returns a geometric hue angle in degrees (0-360) based on RGB values.
	// For neutral colors, hue is undefined and the function will return a quiet NaN value.
	float hue;
	if (rgb[0] == rgb[1] && rgb[1] == rgb[2])
	{
		//hue = FLT_NAN; // RGB triplets where RGB are equal have an undefined hue
		hue = 0;
	}
	else
	{
		hue = (180. / PI) * atan(sqrt(3.0)*(rgb[1] - rgb[2]), 2 * rgb[0] - rgb[1] - rgb[2]);
	}

	if (hue < 0.)
		hue = hue + 360;

	return clamp( hue, 0, 360 );
}

float rgb_2_yc( vec3 rgb, float ycRadiusWeight /*= 1.75*/)
{
	// Converts RGB to a luminance proxy, here called YC
	// YC is ~ Y + K * Chroma
	// Constant YC is a cone-shaped surface in RGB space, with the tip on the 
	// neutral axis, towards white.
	// YC is normalized: RGB 1 1 1 maps to YC = 1
	//
	// ycRadiusWeight defaults to 1.75, although can be overridden in function 
	// call to rgb_2_yc
	// ycRadiusWeight = 1 -> YC for pure cyan, magenta, yellow == YC for neutral 
	// of same value
	// ycRadiusWeight = 2 -> YC for pure red, green, blue  == YC for  neutral of 
	// same value.

	float r = rgb[0]; 
	float g = rgb[1]; 
	float b = rgb[2];
  
	float chroma = sqrt(b*(b-g)+g*(g-r)+r*(r-b));

	return ( b + g + r + ycRadiusWeight * chroma) / 3.;
}

float sigmoid_shaper( float x)
{
	// Sigmoid function in the range 0 to 1 spanning -2 to +2.

	float t = max( 1 - abs( 0.5 * x ), 0 );
	float y = 1 + sign(x) * (1 - t*t);
	return 0.5 * y;
}



float FilmSlope = 0.91;
float FilmToe = 0.53;
float FilmShoulder = 0.23;
float FilmBlackClip = 0;
float FilmWhiteClip = 0.035;

vec3 FilmToneMap( vec3 LinearColor ) 
{
	const mat3 sRGB_2_AP0 = transpose(XYZ_2_AP0_MAT) * (transpose(D65_2_D60_CAT) * transpose(sRGB_2_XYZ_MAT));//mul( XYZ_2_AP0_MAT, mul( D65_2_D60_CAT, sRGB_2_XYZ_MAT ) );
	const mat3 sRGB_2_AP1 = transpose(XYZ_2_AP1_MAT) * (transpose(D65_2_D60_CAT) * transpose(sRGB_2_XYZ_MAT)); //mul( XYZ_2_AP1_MAT, mul( D65_2_D60_CAT, sRGB_2_XYZ_MAT ) );

	const mat3 AP0_2_sRGB = transpose(XYZ_2_sRGB_MAT)* ( transpose(D60_2_D65_CAT) * transpose(AP0_2_XYZ_MAT) );//mul( XYZ_2_sRGB_MAT, mul( D60_2_D65_CAT, AP0_2_XYZ_MAT ) );
	const mat3 AP1_2_sRGB =  transpose(XYZ_2_sRGB_MAT) * ( transpose(D60_2_D65_CAT) * transpose(AP1_2_XYZ_MAT) );//mul( XYZ_2_sRGB_MAT, mul( D60_2_D65_CAT, AP1_2_XYZ_MAT ) );
	
	const mat3 AP0_2_AP1 = transpose(XYZ_2_AP1_MAT) * transpose(AP0_2_XYZ_MAT);//mul( XYZ_2_AP1_MAT, AP0_2_XYZ_MAT );
	const mat3 AP1_2_AP0 = transpose(XYZ_2_AP0_MAT) * transpose(AP1_2_XYZ_MAT);//mul( XYZ_2_AP0_MAT, AP1_2_XYZ_MAT );
	
	vec3 ColorAP1 = LinearColor;
	//vec3 ColorAP1 = mul( sRGB_2_AP1, vec3(LinearColor) );
	


	vec3 ColorAP0 = AP1_2_AP0 * ColorAP1;//mul( AP1_2_AP0, ColorAP1 );


#if 1
	// "Glow" module constants
	const float RRT_GLOW_GAIN = 0.05;
	const float RRT_GLOW_MID = 0.08;

	float saturation = rgb_2_saturation( ColorAP0 );
	float ycIn = rgb_2_yc( ColorAP0 , 1.75);
	float s = sigmoid_shaper( (saturation - 0.4) / 0.2);
	float addedGlow = 1 + glow_fwd( ycIn, RRT_GLOW_GAIN * s, RRT_GLOW_MID);
	ColorAP0 *= addedGlow;
#endif

#if 1
	// --- Red modifier --- //
	const float RRT_RED_SCALE = 0.82;
	const float RRT_RED_PIVOT = 0.03;
	const float RRT_RED_HUE = 0;
	const float RRT_RED_WIDTH = 135;
	float hue = rgb_2_hue( ColorAP0 );
	float centeredHue = center_hue( hue, RRT_RED_HUE );
	float hueWeight = Square( smoothstep( 0, 1, 1 - abs( 2 * centeredHue / RRT_RED_WIDTH ) ) );
		
	ColorAP0.r += hueWeight * saturation * (RRT_RED_PIVOT - ColorAP0.r) * (1. - RRT_RED_SCALE);
#endif
	
	// Use ACEScg primaries as working space
	vec3 WorkingColor = transpose(AP0_2_AP1_MAT) * ColorAP0;//mul( AP0_2_AP1_MAT, ColorAP0 );

	WorkingColor = max( vec3(0, 0, 0), WorkingColor );

	// Pre desaturate
	WorkingColor = mix( vec3(dot( WorkingColor, AP1_RGB2Y )), WorkingColor, 0.96 );
	
	const float ToeScale			= 1 + FilmBlackClip - FilmToe;
	const float ShoulderScale	= 1 + FilmWhiteClip - FilmShoulder;
	
	const float InMatch = 0.18;
	const float OutMatch = 0.18;

	float ToeMatch;
	if( FilmToe > 0.8 )
	{
		// 0.18 will be on straight segment
		ToeMatch = ( 1 - FilmToe  - OutMatch ) / FilmSlope + log10( InMatch );
	}
	else
	{
		// 0.18 will be on toe segment

		// Solve for ToeMatch such that input of InMatch gives output of OutMatch.
		const float bt = ( OutMatch + FilmBlackClip ) / ToeScale - 1;
		ToeMatch = log10( InMatch ) - 0.5 * log( (1+bt)/(1-bt) ) * (ToeScale / FilmSlope);
	}

	float StraightMatch = ( 1 - FilmToe ) / FilmSlope - ToeMatch;
	float ShoulderMatch = FilmShoulder / FilmSlope - StraightMatch;
	
	vec3 LogColor = log10( WorkingColor );
	vec3 StraightColor = FilmSlope * ( LogColor + StraightMatch );
	
	vec3 ToeColor		= (    -FilmBlackClip ) + (2 *      ToeScale) / ( 1 + exp( (-2 * FilmSlope /      ToeScale) * ( LogColor -      ToeMatch ) ) );
	vec3 ShoulderColor	= ( 1 + FilmWhiteClip ) - (2 * ShoulderScale) / ( 1 + exp( ( 2 * FilmSlope / ShoulderScale) * ( LogColor - ShoulderMatch ) ) );

	ToeColor		= any(lessThan(LogColor, vec3(ToeMatch, ToeMatch, ToeMatch)) )?      ToeColor : StraightColor;
	ShoulderColor	= any( greaterThan(LogColor, vec3(ShoulderMatch, ShoulderMatch, ShoulderMatch))) ? ShoulderColor : StraightColor;

	vec3 t = clamp( ( LogColor - ToeMatch ) / ( ShoulderMatch - ToeMatch ) , 0.0, 1.0);
	t = ShoulderMatch < ToeMatch ? 1 - t : t;
	t = (3-2*t)*t*t;
	vec3 ToneColor = mix( ToeColor, ShoulderColor, t );

	// Post desaturate
	ToneColor = mix( vec3 (dot( vec3(ToneColor), AP1_RGB2Y )), ToneColor, 0.93 );

	// Returning positive AP1 values
	return max( vec3(0.0), ToneColor );
}


float sdSquare(vec2 point, float width) {
	vec2 d = abs(point) - width;
	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float vignette(vec2 uv, vec2 size, float roundness, float smoothness) {
	// Center UVs
	uv -= 0.5;

	// Shift UVs based on the larger of width or height
	float minWidth = min(size.x, size.y);
	uv.x = sign(uv.x) * clamp(abs(uv.x) - abs(minWidth - size.x), 0.0, 1.0);
	uv.y = sign(uv.y) * clamp(abs(uv.y) - abs(minWidth - size.y), 0.0, 1.0);

	// Signed distance calculation
	float boxSize = minWidth * (1.0 - roundness);
	float dist = sdSquare(uv, boxSize) - (minWidth * roundness);

	return 1.0 - smoothstep(0.0, smoothness, dist);
}
void main() 
{
	float vignetteValue = vignette(v_texcoord, vec2(0.4, 0.3), 0.3, 0.58);
  	out_Color = vec4(pow(FilmToneMap(texture(RenderTarget1, v_texcoord).rgb), vec3(1.0 / 2.2)) * vignetteValue, 1.0);//vec4( fragColor.rgb, fragColor.a);
}
