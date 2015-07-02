#ifndef TZW_CMC_MODEL_H
#define TZW_CMC_MODEL_H

#include "CMC_Mesh.h"
#include <vector>
#include <QMatrix4x4>
#include "CMC_Material.h"
namespace tzw {
class CMC_Model
{
public:
    CMC_Model();
    ~CMC_Model();
    void addMesh(CMC_Mesh * mesh);
    QMatrix4x4 globalInverseTransform() const;
    void setGlobalInverseTransform(const QMatrix4x4 &globalInverseTransform);
    void writeToFile(const char * fielName);
    void addMaterial(CMC_Material * material);
    void loadFromTZW(const char * fileName);
    std::vector<CMC_Material *> m_materialList;
    std::vector<CMC_Mesh * > m_meshList;
    QMatrix4x4 m_globalInverseTransform;
private:
};

} // namespace tzw

#endif // TZW_CMC_MODEL_H
