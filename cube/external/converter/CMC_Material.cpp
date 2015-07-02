#include "CMC_Material.h"

namespace tzw {

CMC_Material::CMC_Material()
{

}

CMC_Material::~CMC_Material()
{

}

void CMC_Material::write(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.StartObject ();

    writer.String ("ambient_color");
    writer.StartArray ();
    writer.Double (ambientColor.x());
    writer.Double (ambientColor.y());
    writer.Double (ambientColor.z());
    writer.EndArray ();

    writer.String ("diffuse_color");
    writer.StartArray ();
    writer.Double (diffuseColor.x());
    writer.Double (diffuseColor.y());
    writer.Double (diffuseColor.z());
    writer.EndArray ();

    writer.String ("specular_color");
    writer.StartArray ();
    writer.Double (specularColor.x());
    writer.Double (specularColor.y());
    writer.Double (specularColor.z());
    writer.EndArray ();

    writer.String ("diffuse_texture_path");
    writer.String (diffuseTexturePath.c_str ());

    writer.String("normal_map_texture_path");
    writer.String (normalTexturePath.c_str ());

    writer.EndObject ();
}

void CMC_Material::loadFromTZW(const rapidjson::Value &value)
{
    ambientColor =
            QVector3D(value["ambient_color"][0].GetDouble (),value["ambient_color"][1].GetDouble (),value["ambient_color"][2].GetDouble ());
    diffuseColor =
            QVector3D(value["diffuse_color"][0].GetDouble (),value["diffuse_color"][1].GetDouble (),value["diffuse_color"][2].GetDouble ());
    specularColor =
            QVector3D(value["specular_color"][0].GetDouble (),value["specular_color"][1].GetDouble (),value["specular_color"][2].GetDouble ());
    diffuseTexturePath = value["diffuse_texture_path"].GetString ();
    normalTexturePath = value["normal_map_texture_path"].GetString ();
}

} // namespace tzw

