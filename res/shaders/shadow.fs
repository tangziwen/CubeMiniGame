#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec2 v_texcoord;
varying vec3 v_normal_line;
varying vec3 v_world_position;
void main()
{
	gl_FragColor = vec4(1,1,1,1);
	return;
}



