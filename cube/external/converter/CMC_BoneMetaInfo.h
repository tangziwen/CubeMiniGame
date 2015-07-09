#ifndef TZW_CMC_BONEMETAINFO_H
#define TZW_CMC_BONEMETAINFO_H
#include <string>
#include <QMatrix4x4>

namespace tzw {

class CMC_BoneMetaInfo
{
public:
    CMC_BoneMetaInfo();
    std::string name() const;
    void setName(const std::string &name);

    QMatrix4x4 defaultOffset() const;
    void setDefaultOffset(const QMatrix4x4 &defaultOffset);
private:
    std::string m_name;
    QMatrix4x4 m_defaultOffset;

};

} // namespace tzw

#endif // TZW_CMC_BONEMETAINFO_H
