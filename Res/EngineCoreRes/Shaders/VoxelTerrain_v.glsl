#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
uniform mat4 TU_mvMatrix;
uniform mat4 TU_mMatrix;
uniform mat4 TU_normalMatrix;
uniform float TU_roughness;
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;
attribute vec3 a_color;
attribute vec3 a_bc;
attribute vec3 a_mat;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec3 v_worldNormal;
varying vec3 v_color;
varying vec3 v_bc;
varying vec3 v_heightColor;
varying vec3 v_mat;

vec3 terrainHeightColor[] = { vec3(201, 178, 99), vec3(135, 184, 82), vec3(80, 171, 93), vec3(120, 120, 120), vec3(200, 200, 210)};


//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = TU_mvpMatrix * vec4(a_position,1.0);
	
	v_position = (TU_mvMatrix * vec4(a_position,1.0)).xyz;
	v_normal = (TU_normalMatrix * vec4(a_normal,0.0)).xyz;
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
	v_worldPos = (TU_mMatrix * vec4(a_position, 1.0)).xyz;
	v_bc = a_bc;
	v_worldNormal = (TU_mMatrix * vec4(a_normal, 0.0)).xyz;
	
	//calculat height color
	v_heightColor = vec3(1.0, 1.0, 1.0);
	float height = v_worldPos.y;
	float maxHeight = 30.0;
	int index = int(height / maxHeight * 3);

	
	v_heightColor = mix(terrainHeightColor[index], terrainHeightColor[index + 1], height / maxHeight * 3 - index );
	v_heightColor /= 255.0;
	v_color = a_color;
	v_mat = a_mat;
}