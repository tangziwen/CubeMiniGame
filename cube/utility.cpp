#include "utility.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;



bool utility::ReadFile(const char *pFileName, string &outFile)
{
    ifstream f(pFileName);

       bool ret = false;

       if (f.is_open()) {
           string line;
           while (getline(f, line)) {
               outFile.append(line);
               outFile.append("\n");
           }
           f.close();

           ret = true;
       }
       else {
       }
       return ret;
}


float utility::Ang2Radius(float ang)
{
    return 3.14/180.0*ang;
}


const char * utility::GuessFileTYPE(const char *file_name)
{
    std::string a(file_name);
    int pos = a.find_last_of('.');
    static char tmp[100];
    int i = pos + 1;
    int j = 0;
    for(;file_name[i];i++,j++)
    {
        tmp[j] = file_name[i];
    }
    tmp[j] = '\0';
    return tmp;
}


void utility::FindPrefix(const char *file_name,  char *dst)
{
    int pos = -1;
    for(int i = 0;i<strlen(file_name);i++)
    {
        if(file_name[i]=='/')
        {
            pos = i;
        }
    }
    int j = 0;
    for(j = 0;j<=pos;j++)
    {
        dst[j] = file_name[j];
    }
    dst[j]='\0';
}
