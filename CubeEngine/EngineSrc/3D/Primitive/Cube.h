#ifndef TZW_CUBE_H
#define TZW_CUBE_H
#include "../../Base/Node.h"
#include "../../Interface/Drawable3D.h"
#include "../../Texture/TextureMgr.h"
#include "../../Mesh/Mesh.h"
#include "../../Math/vec2.h"
#include <string>
namespace tzw {
/**
 * @brief Cube 类定义了一个在三维空间内显示的立方体
 *
 * Cube类定义了一个在三维空间内显示的立方体，可以用于Debug，也可以用于做一些类似MineCraft的游戏.
 */
class Cube : public Drawable3D
{
public:
    enum class CubeType{
        Cube,
        Skybox,
    };

    Cube();
    void init(Texture * top, Texture * bottom, Texture * side);
    void init(std::string top, std::string bottom, std::string side);
    void init(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename);
    static Cube * create(std::string top,std::string bottom,std::string side);
    static Cube * create(std::string  PosXFilename,
                         std::string  NegXFilename,
                         std::string  PosYFilename,
                         std::string  NegYFilename,
                         std::string  PosZFilename,
                         std::string  NegZFilename);
    virtual void draw();
    CubeType getCubeType() const;
protected:
    CubeType m_cubeType;
    Texture * m_texture[3];
    Texture * m_skyBoxTexture;
    static Mesh * m_mesh[3];
    static Mesh * m_skyBoxMesh;
    Technique * m_technique[3];
    Technique * m_skyBoxTechnique;
private:
    void initSkyBoxMesh();
    void initTopMesh();
    void initBottomMesh();
    void initSideMesh();
};

} // namespace tzw

#endif // TZW_CUBE_H
