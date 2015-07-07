#ifndef TZW_CMC_BONEMETAINFO_H
#define TZW_CMC_BONEMETAINFO_H
#include <string>
#include <QMatrix4x4>

namespace tzw {

class CMC_NodeMetaInfo
{
public:
    CMC_NodeMetaInfo();
    std::string name() const;
    void setName(const std::string &name);

    QMatrix4x4 defaultBoneOffset() const;
    void setDefaultBoneOffset(const QMatrix4x4 &defaultBoneOffset);
private:
    std::string m_name;
    QMatrix4x4 m_defaultBoneOffset;
};

} // namespace tzw

#endif // TZW_CMC_BONEMETAINFO_H
