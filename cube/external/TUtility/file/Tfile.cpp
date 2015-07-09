#include "Tfile.h"
#include "../Data.h"
#include "../log/Tlog.h"
#include "string.h"
#include <algorithm>
namespace tzw
{
Tfile * Tfile::m_instance = nullptr;
Tfile *Tfile::getInstance()
{
    if(!m_instance) m_instance = new Tfile();
    return m_instance;
}

Data Tfile::getData(std::string filename, bool forString)
{
    if (filename.empty())
      {
          return Data::Null;
      }

      Data ret;
      unsigned char* buffer = nullptr;
      size_t size = 0;
      size_t readsize;
      const char* mode = nullptr;

      if (forString)
          mode = "rt";
      else
          mode = "rb";

      do
      {
          // Read the file from hardware
          FILE *fp = fopen(filename.c_str (), mode);

          fseek(fp,0,SEEK_END);
          size = ftell(fp);
          fseek(fp,0,SEEK_SET);

          if (forString)
          {
              buffer = (unsigned char*)malloc(sizeof(unsigned char) * (size + 1));
              buffer[size] = '\0';
          }
          else
          {
              buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
          }

          readsize = fread(buffer, sizeof(unsigned char), size, fp);
          fclose(fp);

          if (forString && readsize < size)
          {
              buffer[readsize] = '\0';
          }
      } while (0);

      if (nullptr == buffer || 0 == readsize)
      {
          Tlog()<<"Get data from file"<<filename.c_str()<<"failed";
      }
      else
      {
          ret.fastSet(buffer, readsize);
      }

      return ret;
}

std::string Tfile::fullPathFromRelativeFile(const std::string &filename, const std::string &relativeFile)
{
    return relativeFile.substr(0, relativeFile.rfind('/')+1) + (filename);
}

void Tfile::findPreFix(const char *fileName, char *dst)
{
        int pos = -1;
        for(int i = 0;i<strlen(fileName);i++)
        {
            if(fileName[i]=='/')
            {
                pos = i;
            }
        }
        int j = 0;
        for(j = 0;j<=pos;j++)
        {
            dst[j] = fileName[j];
        }
        dst[j]='\0';
}

std::string Tfile::guessFileType(std::string filePath)
{
    std::string a(filePath);
    int pos = a.find_last_of('.');
    char tmp[100];
    int i = pos + 1;
    int j = 0;
    for(;filePath[i];i++,j++)
    {
        tmp[j] = filePath[i];
    }
    tmp[j] = '\0';
    return std::string(tmp);
}

void Tfile::addSearchPath(std::string searchPath)
{
    m_searchPath.push_back (searchPath);
}

std::string Tfile::getReleativePath(std::string filePath)
{
    std::string relativePath;
    for(auto i = filePath.rbegin ();i!=filePath.rend ();i++)
    {
        if((*i)=='/' ||(*i)=='\\' )
        {
            break;
        }else
        {
            relativePath.push_back (*i);
        }
    }
    std::reverse(relativePath.begin (),relativePath.end ());
    return relativePath;
}

std::vector<std::string> Tfile::getAbsolutlyFilePath(std::string filePath)
{
    std::vector<std::string> result;
    for(int i =0;i<m_searchPath.size();i++)
    {
        auto str = m_searchPath[i];
        str.append (filePath);
        result.push_back (str);
    }
    return result;
}

Tfile::Tfile()
{

}

Tfile::~Tfile()
{

}


}


