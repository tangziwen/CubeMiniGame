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
    static Tvector uniformHemisphericalVector(Tvector normal);
protected:
    TbaseMath();
    ~TbaseMath();
};
}


#endif // TBASEMATH_H
