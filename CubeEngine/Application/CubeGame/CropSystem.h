#pragma once

#include "Engine/EngineDef.h"
#include "Math/vec3.h"
#include "3D/Vegetation/FoliageSystem.h"
namespace tzw {

class Crop
{
public:
    Crop(std::string name);
    void plantCrop(vec3 pos);
    std::string getName();
    void setStage(int idx, VegetationInfo * stage);
private:
    std::string m_name;
    VegetationInfo * m_stage[3];
    Foliage * m_foliage = nullptr;
};



class CropSystem : public Singleton<CropSystem>
{
public:
    CropSystem();
    void initFromFile();
    void addCropType(Crop * crop);
    void plantCrop(vec3 pos);
    Crop * getCropByName(std::string name);
private:
    std::unordered_map<std::string, Crop *> m_cropList;
};

} // namespace tzw

