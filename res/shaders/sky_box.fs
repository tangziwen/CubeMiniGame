#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec3 v_texcoord;
varying vec3 v_normal_line;
varying vec3 world_position;

uniform samplerCube gCubemapTexture;        
void main()
{
	gl_FragColor = textureCube(gCubemapTexture, v_texcoord);  
	return;
}

