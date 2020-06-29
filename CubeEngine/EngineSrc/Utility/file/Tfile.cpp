#include "Tfile.h"
#include "../log/Log.h"
#include "string.h"
#include <algorithm>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include "Utility/log/Log.h"
#include "zip/zip.h"
#include <filesystem>
namespace tzw
{
Tfile * Tfile::m_instance = nullptr;
Tfile *Tfile::shared()
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
      size_t readsize = 0;
      const char* mode = nullptr;
      if (forString)
          mode = "rt";
      else
          mode = "rb";

	//search the file
	for(auto searchPath :m_searchPath)
	{
		std::string realFileName = searchPath + filename;
          // Read the file from hardware
		FILE *fp = fopen(realFileName.c_str (), mode);	
		if(!fp)
		{
			continue;
		}
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

		if(buffer && readsize)
		{
		  ret.fastSet(buffer, readsize);
			return ret;
		}
	}


	char tmp[256];
	auto theList = {"png", "jpg", "tga"};
	findPreFix(filename.c_str(), tmp);
	std::string preFix = tmp;
	auto found = preFix.find("UITexture");
	bool hasChangeForDDS = false;
	if(found == std::string::npos)
	{	
		//有cook版本的文件
		if(std::find(theList.begin(), theList.end(), getExtension(filename))!= theList.end())
		{
			filename = getFileNameWithOutExtension(filename) +".dds";
			hasChangeForDDS = true;
		}
	}
	//search the zip
	for(auto zipData : m_searchZip)
    {
		struct zip_t *zip = zipData.second;
		if(zip)
		{
    		std::string zipFilename = filename;
			if(zip_entry_open(zip, zipFilename.c_str()) == 0)
			{
				void * buf = nullptr;

		       readsize = zip_entry_size(zip);
				if(forString)
				{
					buf = malloc(sizeof(char) *(readsize + 1));
				}else
				{
					buf = malloc(sizeof(char) * readsize);	
				}
		        auto size_loaded = zip_entry_noallocread(zip, buf, readsize);
				
				if (size_loaded == -1)
				{
					abort();
				}
				buffer = static_cast<unsigned char*>(buf);
				if(forString)
				{
					//use the real size
					buffer[size_loaded] = '\0';
				}
				zip_entry_close(zip);
				ret.fastSet(buffer, readsize);
			}
    		if(buffer)
    		{
   				if(hasChangeForDDS)
				{
					ret.mangledForDDS = true;
				}
    			return ret;
    		}
		}

    }
	if (nullptr == buffer || 0 == readsize)
	{
		tlogError("Bad file :Get data from file %s failed", filename.c_str());
		abort();
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

std::string Tfile::getFolder(std::string filePath)
{
    return filePath.substr(0,filePath.rfind('/')+1);
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

std::string Tfile::toAbsFilePath(std::string filePath, std::string workingCpy)
{
	if(filePath[0] == '.' && filePath[1] == '/')
	{
		filePath.erase(0, 1);
	}
	workingCpy.append(filePath);
	return workingCpy;
}

void Tfile::addSearchZip(std::string zipPath)
{
	struct zip_t *zip = zip_open(zipPath.c_str(), 0, 'r');
	m_searchZip[zipPath] = zip;
	
}

std::string Tfile::getExtension(std::string path)
{

	auto idx = path.rfind('.');

	if(idx != std::string::npos)
	{
	    return path.substr(idx+1);
	}
	else
	{
	    // No extension found
		return "";
	}
}

std::string Tfile::getFileNameWithOutExtension(std::string path)
{
	auto idx = path.rfind('.');

	if(idx != std::string::npos)
	{
	    return path.substr(0,idx);
	}
	else
	{
	    // No extension found
		return path;
	}
}

bool Tfile::isExist(std::string path)
{
	for(auto searchPath :m_searchPath)
	{
		 if(std::filesystem::exists(searchPath + path))
			 return true;
	}
	bool isZipFind = false;
	char tmp[256];
	auto theList = {"png", "jpg", "tga"};
	findPreFix(path.c_str(), tmp);
	std::string preFix = tmp;
	auto found = preFix.find("UITexture");
	bool hasChangeForDDS = false;
	if(found == std::string::npos)
	{	
		//有cook版本的文件
		if(std::find(theList.begin(), theList.end(), getExtension(path))!= theList.end())
		{
			path = getFileNameWithOutExtension(path) +".dds";
			hasChangeForDDS = true;
		}
	}
	//search the zip
	for(auto zipData : m_searchZip)
    {
		struct zip_t *zip = zipData.second;
		if(zip)
		{
    		std::string zipFilename = path;
			if(zip_entry_open(zip, zipFilename.c_str()) == 0)
			{
				isZipFind = true;
				zip_entry_close(zip);
				break;
			}
		}

    }
	return isZipFind;
}

rapidjson::Document Tfile::getJsonObject(std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d",
			filePath.c_str(),
			doc.GetParseError(),
			doc.GetErrorOffset());
		exit(1);
	}
	return doc;
}

Tfile::Tfile()
{

}

Tfile::~Tfile()
{
	for(auto zip :m_searchZip)
	{
		zip_close(zip.second);
	}
	m_searchZip.clear();
}


}


