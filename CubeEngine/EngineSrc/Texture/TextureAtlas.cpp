#include "TextureAtlas.h"
#include "TextureMgr.h"


#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw {

TextureAtlas::TextureAtlas(std::string sheetFile)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(sheetFile,true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	//判断读取成功与否 和 是否为数组类型
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d", sheetFile.c_str(), doc.GetParseError(), doc.GetErrorOffset());
		return;
	}
	auto imagePath = doc["meta"]["image"].GetString();
	auto folder = Tfile::shared()->getFolder(sheetFile);
	setTexture(TextureMgr::shared()->getByPath(folder + imagePath));
	m_width = doc["meta"]["size"]["w"].GetInt();
	m_height = doc["meta"]["size"]["h"].GetInt();
	auto& frames = doc["frames"];

	for(auto i = frames.MemberBegin(); i != frames.MemberEnd(); ++i)
	{
		auto keyStr = i->name.GetString();
		auto frame = new TextureFrame();
		frame->setName(keyStr);
		auto &frameObj = i->value;
		frame->x = frameObj["frame"]["x"].GetInt();
		frame->y = frameObj["frame"]["y"].GetInt();
		frame->w = frameObj["frame"]["w"].GetInt();
		frame->h = frameObj["frame"]["h"].GetInt();
		frame->setSheet(this);
		m_frameList.insert(std::make_pair(keyStr,frame));
	}
}

Texture *TextureAtlas::texture() const
{
	return m_texture;
}

void TextureAtlas::setTexture(Texture *texture)
{
	m_texture = texture;
}

int TextureAtlas::height() const
{
	return m_height;
}

void TextureAtlas::setHeight(int height)
{
	m_height = height;
}

int TextureAtlas::width() const
{
	return m_width;
}

void TextureAtlas::setWidth(int width)
{
	m_width = width;
}

TextureFrame *TextureAtlas::getFrame(std::string name)
{
	auto result = m_frameList.find(name);
	if(result != m_frameList.end())
	{
		return result->second;
	}else
	{
		return nullptr;
	}
}

} // namespace tzw
