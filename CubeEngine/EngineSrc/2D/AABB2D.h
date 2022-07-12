#pragma once
#include "Math/vec2.h"
namespace tzw
{
struct AABB2D
{
    AABB2D(vec2 minVal, vec2 maxVal);
    AABB2D();
    void set(vec2 minVal, vec2 maxVal)
    {
        m_min = minVal;
        m_max = maxVal;
    }
    bool isCanCotain(const AABB2D& aabb)
    {
        if (m_min.x <aabb.m_min.x && m_min.y <aabb.m_min.y
            && m_max.x>aabb.m_max.x && m_max.y>aabb.m_max.y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool isIntersect(const AABB2D& aabb)
    {
        bool isHit= ((m_min.x >= aabb.m_min.x && m_min.x <= aabb.m_max.x) || (aabb.m_min.x >= m_min.x && aabb.m_min.x <= m_max.x)) &&
               ((m_min.y >= aabb.m_min.y && m_min.y <= aabb.m_max.y) || (aabb.m_min.y >= m_min.y && aabb.m_min.y <= m_max.y));
   
        return isHit;
    }

    vec2 m_min = {-99999.f, -99999.f};
    vec2 m_max = {99999.f, 99999.f};
};

}