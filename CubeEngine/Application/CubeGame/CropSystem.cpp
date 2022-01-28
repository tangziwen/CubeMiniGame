#include "CropSystem.h"

#include "EngineSrc/CubeEngine.h"
#include "CubeGame/GameWorld.h"
#include "Game/ConsolePanel.h"
#include "Event/EventMgr.h"
#include "Technique/MaterialPool.h"
#include "3D/Sky.h"
#include "Collision/PhysicsMgr.h"
#include "Utility/math/TbaseMath.h"
#include "BuildingSystem.h"
#include "2D/GUISystem.h"
#include "../../EngineSrc/2D/imnodes.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

#include "UIHelper.h"

#include "dirent.h"
#include "Shader/ShaderMgr.h"
#include "2D/imgui_internal.h"
#include "algorithm"
#include "ThrusterPart.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleInitVelocityModule.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitPosModule.h"
#include "VehicleBroswer.h"
#include "PartSurfaceMgr.h"
#include "Base/TranslationMgr.h"
#include "2D/imgui_markdown.h"
#include "Utility/file/Tfile.h"
#include "LoadingUI.h"
#include "Action/ActionCalFunc.h"
#include "Base/TimerMgr.h"
#include "Engine/WorkerThreadSystem.h"
#include "GameItemMgr.h"
#include "UI/KeyMapper.h"
#include "UI/LoadWorldUI.h"
#include "UI/NewWorldSettingUI.h"
#include "UI/PainterUI.h"


namespace tzw {

CropSystem::CropSystem()
{

}

void CropSystem::initFromFile()
{
	std::string filePath = "Crops/Crops.json";
	auto data = Tfile::shared()->getData(filePath, true);
	rapidjson::Document doc;
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d\n", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
		exit(1);
	}
	if (doc.HasMember("Crops"))
	{
		auto& items = doc["Crops"];
		for (unsigned int i = 0; i < items.Size(); i++)
		{
			auto& item = items[i];
			std::string name = item["name"].GetString();
			Crop * crop = new Crop(name);
			std::string stage0 = item["stage0"]["Foliage"].GetString();
			std::string stage1 = item["stage0"]["Foliage"].GetString();
			std::string stage2 = item["stage0"]["Foliage"].GetString();
			crop->setStage(0, FoliageSystem::shared()->getVegetationInfoByName(stage0));
			crop->setStage(1, FoliageSystem::shared()->getVegetationInfoByName(stage1));
			crop->setStage(2, FoliageSystem::shared()->getVegetationInfoByName(stage2));
			addCropType(crop);
		}
	}

}

void CropSystem::addCropType(Crop* crop)
{
	m_cropList[crop->getName()] = crop;
}

void CropSystem::plantCrop(vec3 pos)
{
	
}

Crop* CropSystem::getCropByName(std::string name)
{
	return m_cropList[name];
}

Crop::Crop(std::string name):m_name(name)
{
}

void Crop::plantCrop(vec3 pos)
{
	tlog("crop!!!plant%s", m_name.c_str());
	if(!m_foliage)
	{
		m_foliage = new Foliage(GameMap::shared()->getGrassId());
		FoliageSystem::shared()->addTreeGroup(m_foliage);
	}
	InstanceData instance;
	Matrix44 mat;
	mat.setTranslate(pos);
	instance.transform = mat;
	instance.extraInfo = vec4(vec3(0, 1, 0), 0);
	m_foliage->m_instance.push_back(instance);
	
}

std::string Crop::getName()
{
	return m_name;
}

void Crop::setStage(int idx, VegetationInfo* stage)
{
	m_stage[idx] = stage;
}

} // namespace tzw
