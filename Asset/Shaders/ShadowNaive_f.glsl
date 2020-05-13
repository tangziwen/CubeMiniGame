
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
uniform sampler2D DiffuseMap;//albedo
varying vec2 v_texcoord;
void main()
{
	vec4 col = texture2D(DiffuseMap,v_texcoord);
	if(col.a <0.5)
	{
		discard;
	}
	//do nothing ,we only write depth
}
//! [0]