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
attribute vec4 a_color;
attribute vec3 a_tangent;
#ifdef FLAG_EnableInstanced
attribute mat4 a_instance_offset;
attribute vec4 a_instance_offset2;
attribute vec4 a_instance_offset3;
#endif
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec4 v_color;
varying vec3 v_worldPos;
varying vec3 v_tangent;
#ifdef FLAG_EnableInstanced
varying vec4 v_instanceColor;
#endif

mat4 fromQ(vec4 q){
    mat4 m;
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;

    float xx2 = q.x * x2;
    float yy2 = q.y * y2;
    float zz2 = q.z * z2;
    float xy2 = q.x * y2;
    float xz2 = q.x * z2;
    float yz2 = q.y * z2;
    float wx2 = q.w * x2;
    float wy2 = q.w * y2;
    float wz2 = q.w * z2;

    m[0][0] = 1.0 - yy2 - zz2;
    m[0][1] = xy2 + wz2;
    m[0][2] = xz2 - wy2;
    m[0][3] = 0.0;

    m[1][0] = xy2 - wz2;
    m[1][1] = 1.0 - xx2 - zz2;
    m[1][2] = yz2 + wx2;
    m[1][3] = 0.0;

    m[2][0] = xz2 + wy2;
    m[2][1] = yz2 - wx2;
    m[2][2] = 1.0 - xx2 - yy2;
    m[2][3] = 0.0;

    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;
    return m;
}

//! [0]
void main()
{
#ifdef FLAG_EnableInstanced

    mat4 modelMatrix = (a_instance_offset);
    // Calculate vertex position in screen space
    gl_Position = TU_mvpMatrix * modelMatrix * vec4(a_position,1.0);
	v_position = (TU_mMatrix * modelMatrix * vec4(a_position,1.0)).xyz;
	v_worldPos = (TU_mMatrix * modelMatrix * vec4(a_position,1.0)).xyz;
    v_normal = (modelMatrix * vec4(a_normal.xyz, 0.0)).xyz;
	v_tangent = (TU_mMatrix * modelMatrix * vec4(a_tangent,0.0)).xyz;
    v_instanceColor = a_instance_offset2;
#else
    // Calculate vertex position in screen space
    gl_Position = TU_mvpMatrix * vec4(a_position,1.0);
	v_position = (TU_mMatrix * vec4(a_position,1.0)).xyz;
	v_normal = (TU_mMatrix * vec4(a_normal,0.0)).xyz;
    v_tangent = (TU_mMatrix * vec4(a_tangent,0.0)).xyz;
	v_worldPos = (TU_mMatrix * vec4(a_position, 1.0)).xyz;
#endif
	v_texcoord = a_texcoord;
	v_color = a_color;
}