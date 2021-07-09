#include "FPGunMgr.h"

#include "FPGunData.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include "FPGun.h"
namespace tzw
{
FPGun* FPGunMgr::loadGun(std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
		abort();
	}
	FPGunData* gunData = new FPGunData();
	gunData->m_filePath = doc["FilePath"].GetString();
	gunData->m_hipPos = vec3(doc["HipPos"][0].GetDouble(), doc["HipPos"][1].GetDouble(), doc["HipPos"][2].GetDouble());
	gunData->m_adsPos = vec3(doc["ADSPos"][0].GetDouble(), doc["ADSPos"][1].GetDouble(), doc["ADSPos"][2].GetDouble());
	gunData->m_rotateE = vec3(doc["Rotate"][0].GetDouble(), doc["Rotate"][1].GetDouble(), doc["Rotate"][2].GetDouble());
	gunData->m_scale = doc["Scale"].GetDouble();
	gunData->m_magCapacity = doc["MagCapacity"].GetInt();
	gunData->m_preserveAmmoCapacity = doc["PreserveAmmoCapacity"].GetInt();
	gunData->m_damage = doc["Damage"].GetInt();
	gunData->m_isAllowADS = doc["IsAllowADS"].GetBool();

	
	FPGun * gun = new FPGun(gunData);
	
	return gun;
}
}
