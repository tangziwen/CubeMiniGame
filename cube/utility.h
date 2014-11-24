#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <string>
#include <QDebug>
#define T_LOG (qDebug())
using namespace std;
namespace utility{
bool ReadFile(const char* pFileName, string& outFile);
float Ang2Radius(float ang);
const char *  GuessFileTYPE(const char * file_name);
void FindPrefix(const char * file_name, char *dst);
}

#endif // UTILITY_H
