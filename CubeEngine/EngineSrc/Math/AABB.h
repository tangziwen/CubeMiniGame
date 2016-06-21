#ifndef TZW_AABB_H
#define TZW_AABB_H
#include "vec3.h"
#include "../Math/Matrix44.h"
#include <vector>
namespace tzw {


class AABB
{
public:
    AABB();
    ~AABB();
    void update(vec3 *vec, int num);
    void update(vec3 vec);
    void transForm(Matrix44 mat);
    void reset();
    void merge(AABB box);
    vec3 min() const;
    void setMin(const vec3 &min);

    vec3 max() const;

    vec3 centre();

    std::vector<AABB> split8();
    bool isInside(vec3 p);
    void setMax(const vec3 &max);
    bool isCanCotain(AABB aabb);
    bool isIntersect(AABB aabb, vec3 &overLap);
private:
    vec3 m_min;
    vec3 m_max;
};

} // namespace tzw

#endif // TZW_AABB_H
