#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <string>
#include <QDebug>
#define T_LOG (qDebug())
#define T_VALID(A) (A!=NULL)
#define T_INVALID(A) (A==NULL)
using namespace std;
namespace utility{
bool ReadFile(const char* pFileName, string& outFile);
float Ang2Radius(float ang);
const char *  GuessFileTYPE(const char * file_name);
void FindPrefix(const char * file_name, char *dst);
}

#endif // UTILITY_H
