#pragma once
#include <string>
#include <vector>
#include "Data.h"
#include <map>
#include "Base/Singleton.h"
#include "Math/vec3.h"
#include "Math/vec4.h"
#include "rapidjson/document.h"
namespace tzw
{
	class JsonUtility: public Singleton<JsonUtility>
	{
	public:
		JsonUtility();
		vec3 getV3(rapidjson::Value& doc);
		void addV3(rapidjson::Value& obj, rapidjson::Document & doc, std::string memberName, vec3 v3);
		void setV3(rapidjson::Value& obj, rapidjson::Document & doc, vec3 v3);

		vec4 getV4(rapidjson::Value& doc);
		void addV4(rapidjson::Value& obj, rapidjson::Document & doc, std::string memberName, vec4 v4);
		void setV4(rapidjson::Value& obj, rapidjson::Document & doc, vec4 v4);
	};
}