#include "TbaseMath.h"

#include <stdlib.h>
namespace tzw {
double TbaseMath::PI = 3.141592654;

double TbaseMath::PI_OVER_180 = TbaseMath::PI / 180.0;

static std::default_random_engine g_RANDOM_ENGINE;
float TbaseMath::Ang2Radius(float angle)
{
    return TbaseMath::PI * angle /180;
}

float TbaseMath::Radius2Ang(float radius)
{
    return radius * 180 / TbaseMath::PI;
}

unsigned int TbaseMath::nextPow2(unsigned int num)
{
    unsigned int rval=1;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval<num) rval<<=1;
    return rval;
}

float TbaseMath::clampf(float value, float lowBound, float upBound)
{
    if(value < lowBound) return lowBound;
    if(value > upBound) return upBound;
    return value;
}

int TbaseMath::clampI(int value, int lowBound, int upBound)
{
    if(value < lowBound) return lowBound;
    if(value > upBound) return upBound;
    return value;
}

void TbaseMath::initRandomEngine()
{
    std::random_device rd;
    g_RANDOM_ENGINE.seed(rd());
}

std::default_random_engine& TbaseMath::getRandomEngine()
{
    return g_RANDOM_ENGINE;
}

float TbaseMath::randF()
{
    int r = rand()%10000;
    return 1.0f * r / 10000;
}

float TbaseMath::randFN()
{
    return (randF()-0.5f)*2;
}

float TbaseMath::randPI()
{
    return randF() * PI;
}

float TbaseMath::rand2PI()
{
    return randF() * PI *2;
}

float TbaseMath::randRange(float lo, float hi)
{
	return lo + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(hi-lo)));
}

int TbaseMath::randRangeInt(int theMinValue, int theMaxValue)
{
    return rand()%(theMaxValue - theMinValue + 1) + theMinValue;
}

Tvector TbaseMath::uniformHemisphericalVector(Tvector normal)
{
    double r1=2*TbaseMath::PI*TbaseMath::randF ();
    double r2=TbaseMath::randF ();
    double r2s=sqrt(r2);
    Tvector w=normal;
    Tvector u;
    if(fabsf(w.x())>0.1f)
    {
        u = Tvector(0,1,0);
    }else
    {
        u = Tvector(1,0,0);
    }
    u = Tvector::crossProduct (u,w);
    u.normalize ();
    Tvector v=Tvector::crossProduct (w,u);
    Tvector dir = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).normalized();
    return dir;
}

TbaseMath::TbaseMath()
{

}

TbaseMath::~TbaseMath()
{

}
}


