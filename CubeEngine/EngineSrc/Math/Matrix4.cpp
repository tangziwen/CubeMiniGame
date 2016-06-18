#include "Matrix4.h"
#include <math.h>
namespace tzw {
/**
 * @brief Matrix4::Matrix4 4x4矩阵的构造函数
 * @param isAutoIdentity 是否需要在创建时设置成单位矩阵，默认值为true
 */
Matrix4::Matrix4(bool isAutoIdentity)
{
    if(isAutoIdentity) setToIdentity();
}
/**
 * @brief Matrix4::setToIdentity 将矩阵设置为单位矩阵
 */
void Matrix4::setToIdentity()
{
    data[0] = 1.0f;
    data[1] = 0.0f;
    data[2] = 0.0f;
    data[3] = 0.0f;

    data[4] = 0.0f;
    data[5] = 1.0f;
    data[6] = 0.0f;
    data[7] = 0.0f;

    data[8] = 0.0f;
    data[9] = 0.0f;
    data[10] = 1.0f;
    data[11] = 0.0f;

    data[12] = 0.0f;
    data[13] = 0.0f;
    data[14] = 0.0f;
    data[15] = 1.0f;
}
/**
 * @brief Matrix4::setTranslate 将该矩阵设为平移矩阵
 * @param translate_v 平移向量
 */
void Matrix4::setTranslate(vec3 translate_v)
{
    setToIdentity();
    data[12] = translate_v.x;
    data[13] = translate_v.y;
    data[14] = translate_v.z;
}

/**
 * @brief Matrix4::setScale 将该矩阵设为缩放矩阵(在原点处按坐标轴缩放)
 * @param scale 缩放向量
 */
void Matrix4::setScale(vec3 scale)
{
    setToIdentity();
    data[0] = scale.x;
    data[5] = scale.y;
    data[10] = scale.z;
}

Matrix4 Matrix4::operator *(const Matrix4 &other)
{
    Matrix4 result;
    //following is the colum first data
    float a00 = data[0], a01 = data[1], a02 = data[2], a03 = data[3],
            a10 = data[4], a11 = data[5], a12 = data[6], a13 = data[7],
            a20 = data[8], a21 = data[9], a22 = data[10], a23 = data[11],
            a30 = data[12], a31 = data[13], a32 = data[14], a33 = data[15];

    float b00 = other.data[0], b01 = other.data[1], b02 = other.data[2], b03 = other.data[3],
            b10 = other.data[4], b11 = other.data[5], b12 = other.data[6], b13 = other.data[7],
            b20 = other.data[8], b21 = other.data[9], b22 = other.data[10], b23 = other.data[11],
            b30 = other.data[12], b31 = other.data[13], b32 = other.data[14], b33 = other.data[15];

    result.data[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
    result.data[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
    result.data[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
    result.data[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
    result.data[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
    result.data[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
    result.data[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
    result.data[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
    result.data[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
    result.data[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
    result.data[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
    result.data[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
    result.data[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
    result.data[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
    result.data[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
    result.data[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;

    return result;
}

vec4 Matrix4::operator *(const vec4 &other)
{
    vec4 result;
    double x = other.x, y = other.y, z = other.z, w = other.w;

    result.x = data[0] * x + data[4] * y + data[8] * z + data[12] * w;
    result.y = data[1] * x + data[5] * y + data[9] * z + data[13] * w;
    result.z = data[2] * x + data[6] * y + data[10] * z + data[14] * w;
    result.w = data[3] * x + data[7] * y + data[11] * z + data[15] * w;

    return result;
}

/**
 * @brief Matrix4::setPerspective 将该矩阵设为透视投影矩阵
 * @param left 近裁面最左端
 * @param right 近裁面最右端
 * @param bottom 近裁面最低端
 * @param top 近裁面最高端
 * @param near 近裁面距离
 * @param far 远裁面距离
 */
void Matrix4::setPerspective(float left, float right, float bottom, float top, float near, float far)
{
    float rl = (right - left),
            tb = (top - bottom),
            fn = (far - near);
    data[0] = (near * 2) / rl;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = (near * 2) / tb;
    data[6] = 0;
    data[7] = 0;
    data[8] = (right + left) / rl;
    data[9] = (top + bottom) / tb;
    data[10] = -(far + near) / fn;
    data[11] = -1;
    data[12] = 0;
    data[13] = 0;
    data[14] = -(far * near * 2) / fn;
    data[15] = 0;
}

/**
 * @brief Matrix4::setPerspective 将该矩阵设为透视投影矩阵
 * @param fovy 竖直方向视场角
 * @param aspect 宽高比
 * @param near 近裁面距离
 * @param far 远裁面距离
 */
void Matrix4::setPerspective(float fovy, float aspect, float near, float far)
{
    float top = near * tan(fovy * 3.14159265 / 360.0),
            right = top * aspect;
    setPerspective(-right, right, -top, top, near, far);
}

/**
 * @brief Matrix4::setOrtho 将该矩阵设为正交投影矩阵
 * @param left 近裁面最左端
 * @param right 近裁面最右端
 * @param bottom 近裁面最低端
 * @param top 近裁面最高端
 * @param near 近裁面距离
 * @param far 远裁面距离
 */
void Matrix4::setOrtho(float left, float right, float bottom, float top, float near, float far)
{
    float rl = (right - left),
            tb = (top - bottom),
            fn = (far - near);
    data[0] = 2 / rl;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 2 / tb;
    data[6] = 0;
    data[7] = 0;
    data[8] = 0;
    data[9] = 0;
    data[10] = -2 / fn;
    data[11] = 0;
    data[12] = -(left + right) / rl;
    data[13] = -(top + bottom) / tb;
    data[14] = -(far + near) / fn;
    data[15] = 1;
}

/**
 * @brief Matrix4::setRotate 将该矩阵设为绕任意轴的旋转矩阵
 * @param angle 旋转角度(弧度制)
 * @param axis 旋转轴
 */
void Matrix4::setRotate(float angle, vec3 axis)
{
    setToIdentity();
    float x = axis.x, y = axis.y, z = axis.z,
            len = sqrt(x * x + y * y + z * z),
            s, c, t,
            b00, b01, b02,
            b10, b11, b12,
            b20, b21, b22;

    if (!len) { return ; }
    if (len != 1) {
        len = 1 / len;
        x *= len;
        y *= len;
        z *= len;
    }

    s = sin(angle);
    c = cos(angle);
    t = 1 - c;

    // Construct the elements of the rotation matrix
    b00 = x * x * t + c; b01 = y * x * t + z * s; b02 = z * x * t - y * s;
    b10 = x * y * t - z * s; b11 = y * y * t + c; b12 = z * y * t + x * s;
    b20 = x * z * t + y * s; b21 = y * z * t - x * s; b22 = z * z * t + c;

    data[0] = b00;
    data[1] = b01;
    data[2] = b02;

    data[4] = b10;
    data[5] = b11;
    data[6] = b12;

    data[7] = b20;
    data[8] = b21;
    data[9] = b22;
}

/**
 * @brief Matrix4::inverse 获取逆矩阵
 * @return 逆矩阵
 */
Matrix4 Matrix4::inverse()
{
    Matrix4 result;
    result.setToIdentity();
    float a00 = data[0], a01 = data[1], a02 = data[2], a03 = data[3],
            a10 = data[4], a11 = data[5], a12 = data[6], a13 = data[7],
            a20 = data[8], a21 = data[9], a22 = data[10], a23 = data[11],
            a30 = data[12], a31 = data[13], a32 = data[14], a33 = data[15],

            b00 = a00 * a11 - a01 * a10,
            b01 = a00 * a12 - a02 * a10,
            b02 = a00 * a13 - a03 * a10,
            b03 = a01 * a12 - a02 * a11,
            b04 = a01 * a13 - a03 * a11,
            b05 = a02 * a13 - a03 * a12,
            b06 = a20 * a31 - a21 * a30,
            b07 = a20 * a32 - a22 * a30,
            b08 = a20 * a33 - a23 * a30,
            b09 = a21 * a32 - a22 * a31,
            b10 = a21 * a33 - a23 * a31,
            b11 = a22 * a33 - a23 * a32,

            d = (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06),
            invDet;

    // Calculate the determinant
    if (!d) { return result; }
    invDet = 1 / d;

    result.data[0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.data[1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.data[2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.data[3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.data[4] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.data[5] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.data[6] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.data[7] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.data[8] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.data[9] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.data[10] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.data[11] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.data[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.data[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.data[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.data[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;
    return result;
}

/**
 * @brief Matrix4::transpose 获取转置矩阵
 * @return 转置矩阵
 */
Matrix4 Matrix4::transpose()
{
    Matrix4 result;
    result.data[0] = data[0];
    result.data[1] = data[4];
    result.data[2] = data[8];
    result.data[3] = data[12];
    result.data[4] = data[1];
    result.data[5] = data[5];
    result.data[6] = data[9];
    result.data[7] = data[13];
    result.data[8] = data[2];
    result.data[9] = data[6];
    result.data[10] = data[10];
    result.data[11] = data[14];
    result.data[12] = data[3];
    result.data[13] = data[7];
    result.data[14] = data[11];
    result.data[15] = data[15];
    return result;
}

/**
 * @brief Matrix4::determinant 获取行列式的值
 * @return 行列式的值
 */
float Matrix4::determinant()
{
    float a00 = data[0], a01 = data[1], a02 = data[2], a03 = data[3],
        a10 = data[4], a11 = data[5], a12 = data[6], a13 = data[7],
        a20 = data[8], a21 = data[9], a22 = data[10], a23 = data[11],
        a30 = data[12], a31 = data[13], a32 = data[14], a33 = data[15];

    return (a30 * a21 * a12 * a03 - a20 * a31 * a12 * a03 - a30 * a11 * a22 * a03 + a10 * a31 * a22 * a03 +
            a20 * a11 * a32 * a03 - a10 * a21 * a32 * a03 - a30 * a21 * a02 * a13 + a20 * a31 * a02 * a13 +
            a30 * a01 * a22 * a13 - a00 * a31 * a22 * a13 - a20 * a01 * a32 * a13 + a00 * a21 * a32 * a13 +
            a30 * a11 * a02 * a23 - a10 * a31 * a02 * a23 - a30 * a01 * a12 * a23 + a00 * a31 * a12 * a23 +
            a10 * a01 * a32 * a23 - a00 * a11 * a32 * a23 - a20 * a11 * a02 * a33 + a10 * a21 * a02 * a33 +
            a20 * a01 * a12 * a33 - a00 * a21 * a12 * a33 - a10 * a01 * a22 * a33 + a00 * a11 * a22 * a33);
}

/**
 * @brief Matrix4::get 获取矩阵指定元素的值
 * @param row 行
 * @param colum 列
 * @return
 */
float Matrix4::get(int row,int colum )
{
    return data[colum*4 +row];
}
/**
 * @brief Matrix4::get 设置矩阵指定元素的值
 * @param row 行
 * @param colum 列
 */
void Matrix4::set(int row, int colum, float new_value)
{
    data[colum*4 +row] = new_value;
}

} // namespace tzw

