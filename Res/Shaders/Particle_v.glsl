#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
uniform mat4 TU_mvMatrix;
uniform mat4 TU_mMatrix;
uniform mat4 TU_vMatrix;
uniform mat4 TU_normalMatrix;
uniform mat4 TU_pMatrix;
uniform mat4 TU_mMatrixInverted;

uniform float TU_roughness;
uniform vec3 TU_camPos;
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;
attribute vec4 a_instance_offset;
attribute vec4 a_instance_offset2;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec4 v_color;

//! [0]
void main()
{

    // Calculate vertex position in screen space
    
	mat4 modelMatrix = TU_mMatrix;

    modelMatrix[3][0] = a_instance_offset.x;
    modelMatrix[3][1] = a_instance_offset.y;
    modelMatrix[3][2] = a_instance_offset.z;

    mat4 modelView = TU_vMatrix * modelMatrix;
    // // First colunm.
    modelView[0][0] = a_instance_offset.w;
    modelView[0][1] = 0.0;
    modelView[0][2] = 0.0;

    // // if (spherical == 1)
    // // {
    // // Second colunm.
    modelView[1][0] = 0.0;
    modelView[1][1] = a_instance_offset.w;
    modelView[1][2] = 0.0;
    // // }

    // // Thrid colunm.
    modelView[2][0] = 0.0;
    modelView[2][1] = 0.0;
    modelView[2][2] = a_instance_offset.w;

    gl_Position = TU_pMatrix * modelView * vec4(a_position,1.0);
    
	v_color = a_instance_offset2;
    v_texcoord = a_texcoord;
}