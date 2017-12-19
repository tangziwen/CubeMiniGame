#ifndef TZW_MODEL_H
#define TZW_MODEL_H

#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
#include "../Effect/Effect.h"
namespace tzw {
class Model : public Drawable3D
{
public:
    friend class ModelLoader;
    Model();
    void initWithFile(std::string modelFilePath);
    static Model * create(std::string modelFilePath);
	void submitDrawCmd() override;
	Mesh * getMesh(int id);
private:
    std::vector<Mesh *> m_meshList;
    std::vector<Material * >m_effectList;
};

} // namespace tzw

#endif // TZW_MODEL_H
