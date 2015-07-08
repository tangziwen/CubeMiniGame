#include "utility.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
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


QQuaternion utility::RotateMatrix2Quaternion(QMatrix4x4 matrix)
{
    QQuaternion result;
    float m[4][4];
    memcpy(m,matrix.data (),sizeof(float)*16);
    float tr, s, q[4];
    int i, j, k;

    int nxt[3] = {1, 2, 0 };
    // 计算矩阵轨迹
    tr = m[1][1] + m[2][2] + m[3][3];

    // 检查矩阵轨迹是正还是负
    if(tr>0.0f)
    {
        s = sqrt(tr + 1.0f);
        result.setScalar (s / 2.0f);
        s = 0.5f / s;
        result.setX ((m[2][3] - m[3][2]) * s);
        result.setY ((m[3][1] - m[1][3]) * s);
        result.setZ ((m[1][2] - m[2][1]) * s);
    }
    else
    {
        // 轨迹是负
        // 寻找m11 m22 m33中的最大分量
        i = 0;
        if(m[1][1]>m[0][0]) i = 1;
        if(m[2][2]>m[i][i]) i = 2;
        j = nxt[i];
        k = nxt[j];

        s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
        q[i] = s * 0.5f;
        if( s!= 0.0f) s = 0.5f / s;
        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] - m[j][i]) * s;
        q[k] = (m[i][k] - m[k][i]) * s;

        QVector4D v4(q[0],q[1],q[2],q[3]);

        result = QQuaternion(v4);
    }
}
