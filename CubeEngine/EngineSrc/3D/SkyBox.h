#ifndef TZW_SKYBOX_H
#define TZW_SKYBOX_H
#include "../Interface/Drawable3D.h"
#include "../Mesh/Mesh.h"
#include "../Texture/TextureMgr.h"
namespace tzw {

class SkyBox : public Drawable3D
{
public:
    SkyBox();
    static SkyBox * create(std::string  PosXFilename,
                         std::string  NegXFilename,
                         std::string  PosYFilename,
                         std::string  NegYFilename,
                         std::string  PosZFilename,
                         std::string  NegZFilename);
    void init(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename);
    Material *getMaterial() const;
    static Mesh *skyBoxMesh();
    void prepare();
protected:
    void initSkyBoxMesh();
    Material * m_skyBoxTechnique;
    static Mesh * m_skyBoxMesh;
    Texture * m_skyBoxTexture;
};

} // namespace tzw

#endif // TZW_SKYBOX_H
