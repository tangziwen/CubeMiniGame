
#include "../Base/Node.h"
#include "PointLight.h"
namespace tzw {

class SpotLight : public PointLight
{
public:
    SpotLight();
    vec3 dir() const;
    void setDir(const vec3 &dir);
	void tick(float dt);
private:
    vec3 m_dir;
};

} // namespace tzw
