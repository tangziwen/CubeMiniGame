#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
uniform mat4 TU_mvMatrix;
uniform mat4 TU_mMatrix;
uniform mat4 TU_vMatrix;
uniform mat4 TU_pMatrix;
uniform mat4 TU_normalMatrix;
uniform float TU_roughness;
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;
attribute vec4 a_color;
attribute vec3 a_tangent;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec4 v_color;
varying vec3 v_worldPos;
varying vec3 v_tangent;



//! [0]
void main()
{
    mat4 modelView = TU_mvMatrix;
    vec3 up = modelView[1].xyz;
    vec3 front = vec3(0, 0, 1);
    if(dot(normalize(up), normalize(front)) > 0.95){
        front = vec3(0, 1, 0);
    }
    vec3 right = normalize(cross(front, up));

    modelView[0][0] = right.x;
    modelView[0][1] = right.y;
    modelView[0][2] = right.z;

    modelView[2][0] = -front.x;
    modelView[2][1] = -front.y;
    modelView[2][2] = -front.z;

    mat4 mvp = TU_pMatrix * modelView;
    
    // Calculate vertex position in screen space
    gl_Position = mvp * vec4(a_position,1.0);
	
	v_position = (mvp * vec4(a_position,1.0)).xyz;
	v_normal = (mvp * vec4(a_normal,0.0)).xyz;
    v_tangent = (mvp * vec4(a_tangent,0.0)).xyz;
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
	v_color = a_color;
	v_worldPos = (mvp * vec4(a_position, 1.0)).xyz;
}