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
uniform float TU_depthBias;
uniform vec3 TU_camPos;
uniform vec3 TU_camDir;
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;
attribute vec4 a_instance_offset;
attribute vec4 a_instance_offset2;
attribute vec4 a_instance_offset3;
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_worldPos;
varying vec4 v_color;




//wrong case
//THIS is Wrong: https://twistedpairdevelopment.wordpress.com/2013/02/11/rotating-a-vector-by-a-quaternion-in-glsl/
//IT mess up the left and right operand
vec4 multQuat_wrong(vec4 q1, vec4 q2)
{
return vec4(
q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z,
q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x,
q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y,
q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
);
}

vec3 rotate_vector_by_quaternion1_wrong( vec4 quat, vec3 vec )
{
vec4 qv = multQuat_wrong( quat, vec4(vec, 0.0) );
return multQuat_wrong( qv, vec4(-quat.x, -quat.y, -quat.z, quat.w) ).xyz;
}

//Right case
vec4 multQuat(vec4 q1, vec4 q2)
{
    return vec4(
    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
    q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
    );
}

vec3 rotate_vector_by_quaternion1( vec4 quat, vec3 vec )
{
vec4 quatInv = vec4(-quat.x, -quat.y, -quat.z, quat.w);
vec4 qv = multQuat( quat, vec4(vec, 0.0) );
return multQuat(qv, quatInv).xyz;
}


//fast case
vec3 rotate_vector_by_quaternion2(vec4 q,vec3 v)
{
    // Extract the vector part of the quaternion
    vec3 u = vec3(q.x, q.y, q.z);

    // Extract the scalar part of the quaternion
    float s = q.w;

    // Do the math
    return 2.0 * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0 * s * cross(u, v);
}

//directorly construct matrix
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

    // Calculate vertex position in screen space
    
	mat4 modelMatrix = TU_mMatrix;




    modelMatrix = fromQ(a_instance_offset3);
    
    // //handle rotation
    // vec3 right = rotate_vector_by_quaternion1_wrong(a_instance_offset3, vec3(1, 0, 0));
    // vec3 up = rotate_vector_by_quaternion1_wrong(a_instance_offset3, vec3(0, 1, 0));
    // vec3 front = rotate_vector_by_quaternion1_wrong(a_instance_offset3, vec3(0, 0, 1));


    // //handle rotation
    // modelMatrix[0][0] = right.x;
    // modelMatrix[0][1] = right.y;
    // modelMatrix[0][2] = right.z;
    // modelMatrix[0][3] = 0;

    // modelMatrix[1][0] = up.x;
    // modelMatrix[1][1] = up.y;
    // modelMatrix[1][2] = up.z;
    // modelMatrix[1][3] = 0;

    // modelMatrix[2][0] = front.x;
    // modelMatrix[2][1] = front.y;
    // modelMatrix[2][2] = front.z;
    // modelMatrix[2][3] = 0;
    

    //handle translation
    modelMatrix[3][0] = a_instance_offset.x;
    modelMatrix[3][1] = a_instance_offset.y;
    modelMatrix[3][2] = a_instance_offset.z;
    modelMatrix[3][3] = 1.0;

    mat4 modelView = TU_vMatrix * modelMatrix;

    vec3 upInView = modelView[1].xyz;
    vec3 frontInView = vec3(0, 0, 1);
    if(dot(normalize(upInView), normalize(frontInView)) < 0.98){
        vec3 rightInView = normalize(cross(frontInView, upInView));

        modelView[0][0] = rightInView.x;
        modelView[0][1] = rightInView.y;
        modelView[0][2] = rightInView.z;

        modelView[2][0] = -frontInView.x;
        modelView[2][1] = -frontInView.y;
        modelView[2][2] = -frontInView.z;

    }

    gl_Position = TU_pMatrix * modelView * vec4(a_position,1.0);
    gl_Position.z -= TU_depthBias / (TU_depthBias + gl_Position.w);
	v_color = a_instance_offset2;
    v_texcoord = a_texcoord;
    vec3 theNormal = -TU_camDir;
    vec3 theright = cross(vec3(0, 1, 0), theNormal);
    vec3 theRealNormal = cross(theright, vec3(0, 1, 0));
    v_normal = vec3(theRealNormal);
}