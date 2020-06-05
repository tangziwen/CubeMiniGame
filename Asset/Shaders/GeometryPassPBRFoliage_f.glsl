#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D DiffuseMap;//albedo
uniform sampler2D MetallicMap;//metallic
uniform sampler2D RoughnessMap;//roughness
uniform sampler2D NormalMap;//normal
uniform vec4 TU_color;
uniform float TU_roughness;
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec4  v_color;
varying vec3 v_worldPos;
varying vec4  v_tangent;

vec3 CalcBumpedNormal(vec3 inputNormal)
{
	vec3 Normal = normalize(inputNormal);
	vec3 Tangent = normalize(v_tangent.xyz);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = cross(Tangent, Normal);
	vec3 BumpMapNormal = texture(NormalMap, v_texcoord).xyz;
	BumpMapNormal.y = 1.0 - BumpMapNormal.y;//we use directx style normal map. 1.0 - Green
	BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
	vec3 NewNormal;
	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	NewNormal = TBN * BumpMapNormal;

	NewNormal = normalize(NewNormal);
	return NewNormal;	
}

//! [0]
void main()
{
	vec3 worldNormal = v_normal;
	#ifdef FLAG_EnableDoubleSide
		if(gl_FrontFacing)
		{
			worldNormal = normalize(v_normal);
		}
		else{
			worldNormal = normalize(-v_normal);
		}
	#endif
    // Set fragment color from texture

	vec4 col = texture(DiffuseMap,v_texcoord);
	if(col.a  <0.5)
	{
		discard;
	}
	vec4 albedo = vec4(pow(col.rgb, vec3(2.2)), col.a)*TU_color * v_color;
	
	float metallic = texture2D(MetallicMap,v_texcoord).r;
	float roughness = texture2D(RoughnessMap,v_texcoord).r;
	gl_FragData[0] = albedo;//vec4(metallic, metallic, metallic, 1.0);
	gl_FragData[1] = vec4(v_position,1.0);
	gl_FragData[2] = vec4(worldNormal,1.0);
	gl_FragData[3] = vec4(1.0,0.0,1.0,1.0);
}
//! [0]