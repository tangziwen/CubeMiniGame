#pragma once
#include "JsonUtility.h"
namespace tzw
{
	JsonUtility::JsonUtility()
	{
	}
	vec3 JsonUtility::getV3(rapidjson::Value& obj)
	{
		return vec3(obj[0].GetDouble(), obj[1].GetDouble(), obj[2].GetDouble() );
	}
	void JsonUtility::addV3(rapidjson::Value& obj, rapidjson::Document & doc, std::string memberName, vec3 v3)
	{
		rapidjson::Value v3_obj(rapidjson::kArrayType);
		v3_obj.PushBack(v3.x, doc.GetAllocator());
		v3_obj.PushBack(v3.y, doc.GetAllocator());
		v3_obj.PushBack(v3.z, doc.GetAllocator());
		rapidjson::Value k(memberName.c_str(), doc.GetAllocator());
		obj.AddMember(k, v3_obj, doc.GetAllocator());
	}
	void JsonUtility::setV3(rapidjson::Value& obj,rapidjson::Document & doc, vec3 v3)
	{
		obj.SetArray();
		obj.PushBack(v3.x, doc.GetAllocator());
		obj.PushBack(v3.y, doc.GetAllocator());
		obj.PushBack(v3.z, doc.GetAllocator());
	}

	vec4 JsonUtility::getV4(rapidjson::Value& obj)
	{
		return vec4(obj[0].GetDouble(), obj[1].GetDouble(), obj[2].GetDouble(), obj[3].GetDouble());
	}

	void JsonUtility::addV4(rapidjson::Value& obj, rapidjson::Document& doc, std::string memberName, vec4 v4)
	{
		rapidjson::Value v4_obj(rapidjson::kArrayType);
		v4_obj.PushBack(v4.x, doc.GetAllocator());
		v4_obj.PushBack(v4.y, doc.GetAllocator());
		v4_obj.PushBack(v4.z, doc.GetAllocator());
		v4_obj.PushBack(v4.w, doc.GetAllocator());
		rapidjson::Value k(memberName.c_str(), doc.GetAllocator());
		obj.AddMember(k, v4_obj, doc.GetAllocator());
	}

	void JsonUtility::setV4(rapidjson::Value& obj, rapidjson::Document& doc, vec4 v4)
	{
		obj.SetArray();
		obj.PushBack(v4.x, doc.GetAllocator());
		obj.PushBack(v4.y, doc.GetAllocator());
		obj.PushBack(v4.z, doc.GetAllocator());
		obj.PushBack(v4.w, doc.GetAllocator());
	}
}
