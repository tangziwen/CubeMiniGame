#include "TextureAtlas.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "TextureMgr.h"
#include "External/TUtility/file/Tfile.h"

namespace tzw {

TextureAtlas::TextureAtlas(std::string sheetFile)
{
    QFile file(sheetFile.c_str());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto jsonDoc = QJsonDocument::fromJson(file.readAll());
    auto rootObj = jsonDoc.object();
    //load the texture first
    auto imagePath = rootObj["meta"].toObject()["image"].toString().toStdString();
    auto folder = Tfile::getInstance()->getFolder(sheetFile);
    setTexture(TextureMgr::shared()->getOrCreateTexture(folder + imagePath));
    m_width = rootObj["meta"].toObject()["size"].toObject()["w"].toInt();
    m_height = rootObj["meta"].toObject()["size"].toObject()["h"].toInt();
    //load all Frame
    auto frames = rootObj["frames"].toObject();
    auto framesKeys = frames.keys();
    for (int i =0;i<framesKeys.size();i++)
    {
        auto keyStr = framesKeys[i].toStdString();
        auto frame = new TextureFrame();
        frame->setName(keyStr);
        auto frameObj = frames[keyStr.c_str()].toObject();
        frame->x = frameObj["frame"].toObject()["x"].toInt();
        frame->y = frameObj["frame"].toObject()["y"].toInt();
        frame->w = frameObj["frame"].toObject()["w"].toInt();
        frame->h = frameObj["frame"].toObject()["h"].toInt();
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
