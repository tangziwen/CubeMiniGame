#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <string>
#include <QDebug>
#include <QQuaternion>
#include <QMatrix4x4>
#define T_LOG (qDebug())
#define T_VALID(A) (A!=NULL)
#define T_INVALID(A) (A==NULL)
#define T_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180
using namespace std;
namespace utility{
bool ReadFile(const char* pFileName, string& outFile);
float Ang2Radius(float ang);
float Radius2Ang(float radius);
const char *  GuessFileTYPE(const char * file_name);
void FindPrefix(const char * file_name, char *dst);
QQuaternion RotateMatrix2Quaternion(QMatrix4x4 matrix);
}

#endif // UTILITY_H
