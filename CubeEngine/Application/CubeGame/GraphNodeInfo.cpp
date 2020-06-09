#include "GraphNodeInfo.h"
#include "2D/imnodes.h"
#include "GameUISystem.h"
#include "Utility/file/Tfile.h"

namespace tzw
{
	GraphNodeInfoMgr::GraphNodeInfoMgr()
	{
		load();
	}

	void GraphNodeInfoMgr::load()
	{
		std::string filePath = "Data/GraphNode/GraphNodeConfig.json";
		auto mData = Tfile::shared()->getData(filePath, true);
		rapidjson::Document matDoc;
		matDoc.Parse<rapidjson::kParseDefaultFlags>(mData.getString().c_str());
		if (matDoc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				matDoc.GetParseError(),
				matDoc.GetErrorOffset());
			abort();
		}
		for(auto i = matDoc.MemberBegin(); i!= matDoc.MemberEnd(); i++)
		{
			auto nameString = i->name.GetString();
			GraphNodeInfo * node = new GraphNodeInfo();
			node->m_title = i->value["Title"].GetString();
			node->m_desc = i->value["desc"].GetString();
			if(i->value.HasMember("Type"))
			{
				std::string typeStr = i->value["Type"].GetString();
				if(typeStr == "Res")
				{
					node->m_type = Node_TYPE_RES;
				}
			}
			node->m_class = atoi(nameString);
			m_infoList.push_back(node);
		}
	}

	GraphNodeInfo* GraphNodeInfoMgr::get(int NodeClass)
	{
		return m_infoList[NodeClass];
	}

	int GraphNodeInfoMgr::amount()
	{
		return m_infoList.size();
	}
}
