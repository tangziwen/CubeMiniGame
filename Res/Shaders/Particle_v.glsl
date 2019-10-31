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
    mat4 modelView = TU_mvMatrix;

    // First colunm.
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

    // vec3  viewerLocal = TU_mMatrixInverted* float4(TU_camPos, 1);
    // vec3  localDir = viewerLocal - float3(0,0,0);

    // localDir.y = localDir.y;//lerp(0, , _VerticalBillboarding);

    // localDir = normalize(localDir);
    // vec3  upLocal = abs(localDir.y) > 0.999f ? float3(0, 0, 1) : float3(0, 1, 0);
    // vec3  rightLocal = normalize(cross(localDir, upLocal));
    // upLocal = cross(rightLocal, localDir);

    vec3 aPos = a_position  + a_instance_offset.xyz;
    // vec3  BBLocalPos = rightLocal * aPos.x + upLocal * aPos.y;
    // gl_Position = TU_pMatrix * modelView * vec4(BBLocalPos,1.0);


    vec3 vpos = (TU_mMatrix *  vec4(aPos, 1.0)).xyz;
    vec4 worldCoord = vec4(TU_mMatrix[3][0], TU_mMatrix[3][1], TU_mMatrix[3][2], 1);
    vec4 viewPos = TU_vMatrix * worldCoord + vec4(vpos.x ,vpos.y, 0, 0);
    gl_Position = TU_pMatrix * modelView *  vec4(aPos, 1.0);
    
	v_color = a_instance_offset2;
    v_texcoord = a_texcoord;
}