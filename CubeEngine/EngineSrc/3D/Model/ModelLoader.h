#ifndef TZW_MODELLOADER_H
#define TZW_MODELLOADER_H
#include "../../Engine/EngineDef.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace tzw {
class Model;
class ModelLoader : public Singleton<ModelLoader>
{
public:
    ModelLoader();
    void loadModel(Model * model,std::string filePath);
};

} // namespace tzw

#endif // TZW_MODELLOADER_H
