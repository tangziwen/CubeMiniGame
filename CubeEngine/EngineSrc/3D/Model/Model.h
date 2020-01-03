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
	void submitDrawCmd(RenderCommand::RenderType passType) override;
	Mesh * getMesh(int id) override;
	Mesh * getMesh() override;
	Material * getMat(int index);
	Material * getMaterial() const override;
	void setMaterial(Material * mat) override;
	void setColor(vec4 color) override;
	std::vector<Mesh * > getMeshList();
	void setMeshList(std::vector<Mesh * > newMeshList);
	int addExtraMeshList(std::vector<Mesh * >newMeshList);
	int getMeshCount() override;
private:
    std::vector<Mesh *> m_meshList;
    std::vector<Material * >m_effectList;
	std::vector<std::vector<Mesh *>> m_extraMeshList;
	int m_currPose;
public:
	int getCurrPose() const;
	void setCurrPose(const int currPose);
};

} // namespace tzw

#endif // TZW_MODEL_H
