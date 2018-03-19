#ifndef TZW_EFFECT_H
#define TZW_EFFECT_H
#include "EngineSrc/Shader/ShaderProgram.h"
#include <map>
#include "EngineSrc/Technique/TechniqueVar.h"
namespace tzw {
class Drawable;
class Material;
class Effect
{
public:
    Effect();
protected:
    ShaderProgram * m_program;
};
} // namespace tzw

#endif // TZW_EFFECT_H
