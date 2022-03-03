#ifndef TBASEMATH_H
#define TBASEMATH_H
#include <math.h>
#include "Tvector.h"
#include <random>
namespace tzw{
class TbaseMath
{
public:
    static double PI;
	static double PI_OVER_180;
    static float randF();
    static float randFN();
    static float randPI();
    static float rand2PI();
	static float randRange(float lo, float hi);
    static int randRangeInt(int theMinValue,int theMaxValue);
    static Tvector uniformHemisphericalVector(Tvector normal);
    static float Ang2Radius(float angle);
    static float Radius2Ang(float radius);
    static unsigned int nextPow2(unsigned int num);
    static float clampf(float value, float lowBound, float upBound);
    static int clampI(int value, int lowBound, int upBound);
    static void initRandomEngine();
    static std::default_random_engine & getRandomEngine();
protected:
    TbaseMath();
    ~TbaseMath();
};
}


#endif // TBASEMATH_H
