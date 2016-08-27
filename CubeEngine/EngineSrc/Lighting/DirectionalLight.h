#ifndef TZW_DIRECTIONALLIGHT_H
#define TZW_DIRECTIONALLIGHT_H

#include "../Base/Node.h"
#include "BaseLight.h"
namespace tzw {

class DirectionalLight : public BaseLight
{
public:
    DirectionalLight();
    vec3 dir() const;
    void setDir(const vec3 &dir);
private:
    vec3 m_dir;
};

} // namespace tzw

#endif // TZW_DIRECTIONALLIGHT_H
