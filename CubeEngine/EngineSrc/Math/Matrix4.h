#ifndef TZW_MATRIX4_H
#define TZW_MATRIX4_H

#include "vec3.h"
#include "vec4.h"
namespace tzw {
/**
 * @brief 该类定义了一个4x4的矩阵
 *
 * 该矩阵可用于进行一般的齐次变换（平移，旋转，缩放），请注意该矩阵内部的存储形式是列主序的，可以直接通过data成员获取值
 */
class Matrix4
{
public:
    Matrix4(bool isAutoIdentity = true);
    void setToIdentity();
    void setTranslate(vec3 translate_v);
    void setScale(vec3 scale);
    Matrix4 operator*(const Matrix4 & other);
    vec4 operator*(const vec4 & other);
    void setPerspective(float left, float right, float bottom, float top, float near, float far);
    void setPerspective(float fovy, float aspect, float near, float far);
    void setOrtho(float left, float right, float bottom, float top, float near, float far);
    void setRotate(float angle,vec3 axis);
    Matrix4 inverse();
    Matrix4 transpose();
    float determinant();
    float get(int row, int colum);
    void set(int row, int colum, float new_value);
    float data[16];
};

} // namespace tzw

#endif // TZW_MATRIX4_H
