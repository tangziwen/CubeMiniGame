#include "BlockInfoMgr.h"
#include <QJsonDocument>
#include <QFile>
#include <QVariantMap>
namespace tzw {

BlockInfoMgr * BlockInfoMgr::m_instance = nullptr;
BlockInfoMgr *BlockInfoMgr::shared()
{
    if(!m_instance)
    {
        m_instance = new BlockInfoMgr;
    }
    return m_instance;
}

BlockInfo *BlockInfoMgr::createOrGetInfo(std::string configFile)
{
    auto result = m_infoMap.find(configFile);
    if(result!=m_infoMap.end())
    {
        return result->second;
    }else
    {
        auto newInfo = new BlockInfo();
        loadInfo(newInfo,configFile);
        m_infoMap.insert(std::make_pair(configFile,newInfo));
        return newInfo;
    }
}

void BlockInfoMgr::loadInfo(BlockInfo *info, std::string configFile)
{
    std::string pre_fix = "./Res/User/CubeGame/Blocks/";
    pre_fix.append(configFile);
    QFile file(pre_fix.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error == QJsonParseError::NoError) {
        if (jsonDocument.isObject()) {
            QVariantMap result = jsonDocument.toVariant().toMap();
            info->setTopFile(result["top"].toString().toStdString());
            info->setBottomFile(result["bottom"].toString().toStdString());
            info->setSideFile(result["side"].toString().toStdString());

            info->finish();
        }
    } else {
        qFatal(error.errorString().toUtf8().constData());
        exit(1);
    }
}

BlockInfoMgr::BlockInfoMgr()
{

}

} // namespace tzw

