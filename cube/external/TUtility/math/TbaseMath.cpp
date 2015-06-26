#include "TbaseMath.h"

#include <stdlib.h>
namespace tzw {
double TbaseMath::PI = 3.141592654;

float TbaseMath::randF()
{
    int r = rand()%100;
    return 1.0f * r /100;
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

Tvector TbaseMath::uniformHemisphericalVector(Tvector normal)
{
    double r1=2*TbaseMath::PI*TbaseMath::randF ();
    double r2=TbaseMath::randF ();
    double r2s=sqrt(r2);
    Tvector w=normal;
    Tvector u;
    if(fabs(w.x())>0.1)
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


