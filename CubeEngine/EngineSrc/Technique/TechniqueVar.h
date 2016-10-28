#ifndef TZW_TECHNIQUEVAR_H
#define TZW_TECHNIQUEVAR_H

#include "../Math/vec2.h"
#include "../Math/vec3.h"
#include "../Math/vec4.h"
#include "../Texture/Texture.h"
#include "../Math/Matrix44.h"
namespace tzw {

struct TechniqueVar
{
    enum class Type
    {
        Invalid,
        Matrix,
        Float,
        Integer,
        Vec4,
        Vec3,
        Vec2,
        Texture
    };

    struct {
        Matrix44 m;
        float f;
        int i;
        vec3 v3;
        vec2 v2;
        vec4 v4;
        Texture *tex;
    }data;
    Type type;
    void setT(Texture * tex);
    void setF(float value);
    void setM(const Matrix44 &value);
    void setI(int value);
    void setV2(vec2 value);
    void setV3(vec3 value);
    void setV4(vec4 value);
    TechniqueVar();
    TechniqueVar * clone() const;
};

} // namespace tzw

#endif // TZW_TECHNIQUEVAR_H
