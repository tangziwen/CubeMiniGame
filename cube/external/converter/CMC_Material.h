#ifndef TZW_CMC_MATERIAL_H
#define TZW_CMC_MATERIAL_H

#include "external/rapidjson/document.h"
#include "external/rapidjson/prettywriter.h"
#include "external/rapidjson/filewritestream.h"
#include "external/rapidjson/stringbuffer.h"

#include <qvector3d.h>
#include <string>
namespace tzw {

struct CMC_Material
{
public:
    CMC_Material();
    ~CMC_Material();
    QVector3D ambientColor;
    QVector3D diffuseColor;
    QVector3D specularColor;
    std::string diffuseTexturePath;
    std::string normalTexturePath;
    void write(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
    void loadFromTZW(const rapidjson::Value &value);
};

} // namespace tzw

#endif // TZW_CMC_MATERIAL_H
