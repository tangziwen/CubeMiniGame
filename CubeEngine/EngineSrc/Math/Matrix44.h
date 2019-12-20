#ifndef TZW_MATRIX44_H
#define TZW_MATRIX44_H
#include "vec3.h"
#include "vec4.h"
#include "Quaternion.h"
namespace tzw {

class Matrix44
{
public:
    Matrix44();
    void setToIdentity();
    void translate(vec3 offset);
    void scale(vec3 scaleFactor);
    void rotate(const Quaternion &quaternion);
    Matrix44 operator *(const Matrix44 &  other) const;
    vec4 operator *(const vec4 &  other) const;
    void ortho(float left, float right, float bottom, float top, float near, float far);
    void perspective(float fovy, float aspect, float near, float far);
    Matrix44 inverted(bool *invertible = 0);
    Matrix44 transpose();
    vec4 operator * (const vec4 & v );
    vec3 transformVec3(vec3 v);
	vec4 transofrmVec4(vec4 v);
    void frustum(float left, float right, float bottom, float top, float near, float far);
    float * data();
	void getRowData(float * data);
	void copyFromArray(float * data);
    vec3 up();
    vec3 forward();
    vec3 right();
	void stripScale();
	vec3 getTranslation();
	float determinant() const;
	bool decompose(vec3* scale, Quaternion* rotation, vec3* translation) const;
private:
    float m_data[16];
};

} // namespace tzw

#endif // TZW_MATRIX44_H
