#include "AABB.h"
#include "math.h"


namespace tzw {

AABB::AABB()
{
    reset();
}

AABB::~AABB()
{

}

/**
 * @brief 将当前AABB八等分，返回八个子AABB
 * @note 可以用来构建八叉树
 * @return 八个子AABB
 */
std::vector<AABB> AABB::split8()
{
    AABB aabb_list[8];
    std::vector<AABB> result;
    auto the_centre = centre();

    //bottom
    //#1
    aabb_list[0].update(m_min);
    aabb_list[0].update(the_centre);
    //#2
    aabb_list[1].update(vec3(the_centre.x,m_min.y,m_min.z));
    aabb_list[1].update(vec3(m_max.x,the_centre.y,the_centre.z));
    //#3
    aabb_list[2].update(vec3(the_centre.x,m_min.y,the_centre.z));
    aabb_list[2].update(vec3(m_max.x,the_centre.y,m_max.z));
    //#4
    aabb_list[3].update(vec3(m_min.x,m_min.y,the_centre.z));
    aabb_list[3].update(vec3(the_centre.x,the_centre.y,m_max.z));

    //top
    //#5
    aabb_list[4].update(vec3(m_min.x,the_centre.y,m_min.z));
    aabb_list[4].update(vec3(the_centre.x,m_max.y,the_centre.z));
    //#6
    aabb_list[5].update(vec3(the_centre.x,the_centre.y,m_min.z));
    aabb_list[5].update(vec3(m_max.x,m_max.y,the_centre.z));
    //#7
    aabb_list[6].update(vec3(the_centre.x,the_centre.y,the_centre.z));
    aabb_list[6].update(vec3(m_max.x,m_max.y,m_max.z));
    //#8
    aabb_list[7].update(vec3(m_min.x,the_centre.y,the_centre.z));
    aabb_list[7].update(vec3(the_centre.x,m_max.y,m_max.z));

    for(int i =0;i<8;i++)
    {
        result.push_back(aabb_list[i]);
    }
    return result;
}

bool AABB::isIntersect(AABB aabb, vec3& overLap)
{
    bool isHit= ((m_min.x >= aabb.m_min.x && m_min.x <= aabb.m_max.x) || (aabb.m_min.x >= m_min.x && aabb.m_min.x <= m_max.x)) &&
           ((m_min.y >= aabb.m_min.y && m_min.y <= aabb.m_max.y) || (aabb.m_min.y >= m_min.y && aabb.m_min.y <= m_max.y)) &&
           ((m_min.z >= aabb.m_min.z && m_min.z <= aabb.m_max.z) || (aabb.m_min.z >= m_min.z && aabb.m_min.z <= m_max.z));
    if(isHit)
    {
        if(min().x<aabb.min().x || max().x >aabb.max().x)
        {
            float offset_x_1 =aabb.max().x - min().x,offset_x_2 = aabb.min().x - max().x;
            if(fabs(offset_x_1)< fabs(offset_x_2))
            {
                overLap.x = offset_x_1;
            }else{
                overLap.x = offset_x_2;
            }
        }
        if(min().y<aabb.min().y || max().y >aabb.max().y)
        {
            float offset_y_1 =aabb.max().y - min().y,offset_y_2 = aabb.min().y - max().y;

            if(fabs(offset_y_1)< fabs(offset_y_2))
            {
                overLap.y = offset_y_1;
            }else{
                overLap.y = offset_y_2;
            }

        }
        if(min().z<aabb.min().z || max().z >aabb.max().z)
        {
            float offset_z_1 =aabb.max().z - min().z,offset_z_2 = aabb.min().z - max().z;

            if(fabs(offset_z_1)< fabs(offset_z_2))
            {
                overLap.z = offset_z_1;
            }else{
                overLap.z = offset_z_2;
            }
        }

    }
    return isHit;
}

} // namespace tzw










