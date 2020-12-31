#ifndef TZW_AABB_H
#define TZW_AABB_H
#include "vec3.h"
#include "../Math/Matrix44.h"
#include <vector>
#include <algorithm>
namespace tzw {


class AABB
{
public:
    AABB();
    ~AABB();
    void update(vec3* vec, int num)
    {
        for (size_t i = 0; i < num; i++)
        {
            // Leftmost point.
            if (vec[i].x < m_min.x)
                m_min.setX(vec[i].x);

            // Lowest point.
            if (vec[i].y < m_min.y)
                m_min.setY(vec[i].y);

            // Farthest point.
            if (vec[i].z < m_min.z)
                m_min.setZ(vec[i].z);

            // Rightmost point.
            if (vec[i].x > m_max.x)
                m_max.setX(vec[i].x);


            // Highest point.
            if (vec[i].y > m_max.y)
                m_max.setY(vec[i].y);

            // Nearest point.
            if (vec[i].z > m_max.z)
                m_max.setZ(vec[i].z);
        }
    }
    void update(vec3 vec)
    {
        // Leftmost point.
        if (vec.x < m_min.x)
            m_min.setX(vec.x);

        // Lowest point.
        if (vec.y < m_min.y)
            m_min.setY(vec.y);

        // Farthest point.
        if (vec.z < m_min.z)
            m_min.setZ(vec.z);

        // Rightmost point.
        if (vec.x > m_max.x)
            m_max.setX(vec.x);


        // Highest point.
        if (vec.y > m_max.y)
            m_max.setY(vec.y);

        // Nearest point.
        if (vec.z > m_max.z)
            m_max.setZ(vec.z);
    }
    void transForm(Matrix44 mat)
    {

        vec3 corners[8];
        // Near face, specified counter-clockwise
        // Left-top-front.
        corners[0] = vec3(m_min.x, m_max.y, m_max.z);
        // Left-bottom-front.
        corners[1] = vec3(m_min.x, m_min.y, m_max.z);
        // Right-bottom-front.
        corners[2] = vec3(m_max.x, m_min.y, m_max.z);
        // Right-top-front.
        corners[3] = vec3(m_max.x, m_max.y, m_max.z);

        // Far face, specified clockwise
        // Right-top-back.
        corners[4] = vec3(m_max.x, m_max.y, m_min.z);
        // Right-bottom-back.
        corners[5] = vec3(m_max.x, m_min.y, m_min.z);
        // Left-bottom-back.
        corners[6] = vec3(m_min.x, m_min.y, m_min.z);
        // Left-top-back.
        corners[7] = vec3(m_min.x, m_max.y, m_min.z);
        for (int i = 0; i < 8; i++)
        {
            vec4 result = mat * vec4(corners[i].x, corners[i].y, corners[i].z, 1);
            corners[i] = vec3(result.x, result.y, result.z);
        }
        reset();
        update(corners, 8);
    }
    void reset()
    {
        m_min = vec3(999999, 999999, 999999);
        m_max = vec3(-999999, -999999, -999999);
    }
    void merge(AABB box)
    {
        // Calculate the new minimum point.
        m_min.setX(std::min(m_min.x, box.min().x));
        m_min.setY(std::min(m_min.y, box.min().y));
        m_min.setZ(std::min(m_min.z, box.min().z));

        // Calculate the new maximum point.
        m_max.setX(std::max(m_max.x, box.max().x));
        m_max.setY(std::max(m_max.y, box.max().y));
        m_max.setZ(std::max(m_max.z, box.max().z));
    }
    vec3 min() const
    {
        return m_min;
    }
    void setMin(const vec3& min)
    {
        m_min = min;
    }

    vec3 max() const
    {
        return m_max;
    }

    vec3 centre()
    {
        return (m_max + m_min) / 2;
    }

    std::vector<AABB> split8();
    bool isInside(vec3 p) const
    {
        if (p.x >= m_min.x && p.y >= m_min.y && p.z >= m_min.z
            && p.x <= m_max.x && p.y <= m_max.y && p.z <= m_max.z)
        {
            return true;
        }
        else {
            return false;
        }
    }
    void setMax(const vec3& max)
    {
        m_max = max;
    }
    bool isCanCotain(AABB aabb)
    {
        if (m_min.x <aabb.min().x && m_min.y <aabb.min().y && m_min.z <aabb.min().z
            && m_max.x>aabb.max().x && m_max.y>aabb.max().y && m_max.z>aabb.max().z)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool isIntersect(AABB aabb, vec3 &overLap);
    vec3 half()
    {
        return size() / 2.0f;
    }
    vec3 size()
    {
        return max() - min();
    }
private:
    vec3 m_min;
    vec3 m_max;
};

} // namespace tzw

#endif // TZW_AABB_H
