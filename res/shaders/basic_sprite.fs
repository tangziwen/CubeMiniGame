#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
varying vec2 v_texcoord;
uniform sampler2D g_diffuse_texture;

void main()
{
	gl_FragColor = texture2D(g_diffuse_texture,v_texcoord);
	return;
}

