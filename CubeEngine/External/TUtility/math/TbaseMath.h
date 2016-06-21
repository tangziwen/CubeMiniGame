#ifndef TBASEMATH_H
#define TBASEMATH_H
#include <math.h>
#include "Tvector.h"
namespace tzw{
class TbaseMath
{
public:
    static double PI;
    static float randF();
    static float randFN();
    static float randPI();
    static float rand2PI();
    static int randRangeInt(int theMinValue,int theMaxValue);
    static Tvector uniformHemisphericalVector(Tvector normal);
    static float Ang2Radius(float angle);
    static float Radius2Ang(float radius);
    static unsigned int nextPow2(unsigned int num);
    static clampf(float value, float lowBound, float upBound);
protected:
    TbaseMath();
    ~TbaseMath();
};
}


#endif // TBASEMATH_H
