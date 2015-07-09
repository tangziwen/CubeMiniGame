#ifndef TFILE_H
#define TFILE_H
#include <string>
#include <vector>
class Data;

namespace tzw
{
class Tfile
{
public:
    static Tfile * getInstance();
    Data getData(std::string filename, bool forString);
    std::string fullPathFromRelativeFile(const std::string &filename, const std::string &relativeFile);
    void findPreFix(const char * fileName, char * dst);
    std::string guessFileType(std::string filePath);
    void addSearchPath(std::string searchPath);
    std::string getReleativePath(std::string filePath);
    std::vector<std::string> getAbsolutlyFilePath(std::string filePath);
private:
    Tfile();
    ~Tfile();
    std::vector<std::string> m_searchPath;
    static Tfile * m_instance;
};
}


#endif // TFILE_H
