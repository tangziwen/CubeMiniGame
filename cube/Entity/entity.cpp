#include "Entity.h"
#include <iostream>
#include "geometry/mesh.h"
#include "utility.h"
#include "string.h"
#include "base/bonedata.h"
#include "material/materialpool.h"
#include "texture/texturepool.h"
#include "shader/shader_program.h"
#include "external/TUtility/TUtility.h"
#include "external/converter/Loader.h"
#include <QDebug>

Entity::Entity()
{
    this->mesh_list.clear();
    this->m_animateTime = 0;
    this->m_hasAnimation = 0;
    this->setIsEnableShadow(true);
    this->onRender = nullptr;
    this->setNodeType (NODE_TYPE_ENTITY);
    this->setShaderProgram (ShaderPool::getInstance ()->get ("default"));
    this->m_currentAnimateIndex = -1;
    this->m_skeleton = nullptr;
    m_isAABBDirty = true;
}

Entity::Entity(const char *file_name,LoadPolicy policy)
{
    this->mesh_list.clear();
    this->m_animateTime = 0;
    this->setIsEnableShadow(true);
    this->onRender = nullptr;
    this->setShaderProgram (ShaderPool::getInstance ()->get ("default"));
    this->m_currentAnimateIndex = -1;
    this->m_skeleton = nullptr;
    this->setNodeType (NODE_TYPE_ENTITY);
    switch(policy)
    {
    case LoadPolicy::LoadFromLoader:
    {

        tzw::Loader loader;
        loader.loadFromModel (file_name);
        loadModelData (loader.model (),file_name);
        setModelData (loader.model ());
    }
        break;
    case LoadPolicy::LoadFromTzw:
    {
        tzw::CMC_ModelData * model = new tzw::CMC_ModelData;
        model->loadFromTZW (file_name);
        loadModelData (model,file_name);
        setModelData (model);
    }
        break;
    }
    m_isAABBDirty = true;
}

void Entity::addMesh(TMesh *mesh)
{
    this->mesh_list.push_back(mesh);
}

TMesh *Entity::getMesh(int index)
{
    return mesh_list[index];
}

void Entity::draw(bool withoutexture)
{
    for(int i=0;i<mesh_list.size();i++)
    {
        TMesh * mesh =mesh_list[i];
        MeshDrawComand * command = mesh->getCommand();
        command->synchronizeData(program,mesh->getMaterial(),mesh->getVerticesVbo(),mesh->getIndicesVbo());
        command->CommandSetRenderState(NULL,NULL,-1,-1,withoutexture);
        command->Draw();
    }
}

void Entity::setShaderProgram(ShaderProgram *program)
{
    this->program = program;
}

void Entity::setCamera(Camera *camera)
{
    this->camera = camera;
}

Camera *Entity::getCamera()
{
    return this->camera;
}

ShaderProgram *Entity::getShaderProgram()
{
    return this->program;
}

void Entity::playAnimate(int index, float time)
{
    setAnimateTime (time);
    setCurrentAnimateIndex (index);
}

float Entity::animateTime() const
{
    return m_animateTime;
}

void Entity::setAnimateTime(float animateTime)
{
    m_animateTime = animateTime;
}
bool Entity::hasAnimation() const
{
    return m_hasAnimation;
}

void Entity::setHasAnimation(bool hasAnimation)
{
    m_hasAnimation = hasAnimation;
}
bool Entity::isEnableShadow() const
{
    return m_isEnableShadow;
}

void Entity::setIsEnableShadow(bool isEnableShadow)
{
    m_isEnableShadow = isEnableShadow;
}

AABB Entity::getAABB()
{
    if(m_isAABBDirty)
    {
        for(int i =0;i<mesh_list.size ();i++)
        {
            m_aabb.merge (mesh_list[i]->aabb());
        }
        m_aabb.transForm (getModelTrans ());
        m_isAABBDirty = false;
    }
    return m_aabb;
}

float Entity::getDistToCamera()
{
    QMatrix4x4 viewMat  = camera->getViewMatrix ();
    QMatrix4x4 transform = getModelTrans ();
    float globalZ = -1*(viewMat.data ()[2] * transform.data ()[12] + viewMat.data ()[6] * transform.data ()[13] + viewMat.data ()[10] * transform.data ()[14] + viewMat.data ()[14]);
    return globalZ;
}

void Entity::adjustVertices()
{

}

void Entity::loadModelData(tzw::CMC_ModelData * cmc_model,const char * file_name)
{
    m_hasAnimation = cmc_model->hasAnimation ();
    char str[100]={'\0'};
    utility::FindPrefix(file_name,str);
    //load material
    loadMaterial(cmc_model,file_name,str);

    //global inverse transform(not supported yet)
    m_globalInverseTransform.InitIdentity ();

    for(int i =0 ;i< cmc_model->meshList ().size ();i++)
    {
        auto the_mesh = cmc_model->meshList ()[i];
        TMesh * mesh =new TMesh();
        this->addMesh(mesh);
        //set material
        mesh->setMaterial(this->material_list[the_mesh->materialIndex ()]);
        for(int j =0; j<the_mesh->m_vertices.size();j++)
        {
            tzw::CMC_Vertex v = the_mesh->m_vertices[j];
            VertexData vec;
            vec.position = v.pos ();
            vec.normalLine = v.normal ();
            vec.texCoord = v.UV ();
            vec.tangent = v.tangent ();
            if(m_hasAnimation)
            {
                for(int i =0;i<4;i++)
                {
                    vec.boneId[i] = v.m_boneIds[i];
                    vec.boneWeight[i] = v.m_boneWeights[i];
                }
            }
            mesh->pushVertex(vec);
        }
        for (unsigned int k = 0 ; k < the_mesh->m_indices.size (); k++) {
            mesh->pushIndex (the_mesh->m_indices[k]);
        }
        mesh->finish();
    }
    m_skeleton = new Skeleton(this,cmc_model->m_BoneMetaInfoMapping.size ());
    createNode(this,cmc_model->rootBone ());
}

void Entity::loadMaterial(tzw::CMC_ModelData *model, const char *file_name, const char *pre_fix)
{
    //store material
    for(int i = 0 ;i<model->materialList ().size();i++)
    {
        auto the_material = model->materialList ()[i];
        char file_postfix[100];
        sprintf(file_postfix,"%s_%d",file_name,i+1);
        Material * material = MaterialPool::getInstance()->createOrGetMaterial(file_postfix);
        MaterialChannel * ambient_channel =  material->getAmbient();
        MaterialChannel * diffuse_channel =  material->getDiffuse();
        MaterialChannel * specular_channel =  material->getSpecular();

        ambient_channel->color = the_material->ambientColor;
        diffuse_channel->color = the_material->diffuseColor;
        specular_channel->color = the_material->specularColor;

        auto normalPath = the_material->normalTexturePath;
        auto normalTexture = loadTextureFromMaterial(normalPath,pre_fix);
        // we don't check the normal texture whether is a nullptr, because models which don't use normal mapping technique are very common.
        material->setNormalMap (normalTexture);

        //now ,we only use the first diffuse texture, will fix it later
        auto diffuse_Path = the_material->diffuseTexturePath;
        auto diffuseTexture = loadTextureFromMaterial(diffuse_Path,pre_fix);
        //if the diffuseTexture is not exist, we will use a default texture to replace.
        if(!diffuseTexture) diffuseTexture = TexturePool::getInstance ()->createOrGetTexture ("default");
        diffuse_channel->texture = diffuseTexture;

        this->material_list.push_back(material);
    }
}

Texture *Entity::loadTextureFromMaterial(std::string fileName, const char *pre_fix)
{
    Texture * result = nullptr;
    if(fileName.empty ())
    {
        return result;
    }else
    {
        char str[100];
        Texture * tmp_tex = TexturePool::getInstance()->createOrGetTexture(fileName.c_str ());
        if(tmp_tex)
        {
            result =tmp_tex;
        }
        fileName = tzw::Tfile::getInstance ()->getReleativePath (fileName);
        sprintf(str,"%s%s",pre_fix,fileName.c_str());
        tmp_tex = TexturePool::getInstance()->createOrGetTexture(str);
        if(tmp_tex)
        {
            result =tmp_tex;
        }
        sprintf(str,"%s%s","res/texture/",fileName.c_str ());
        tmp_tex = TexturePool::getInstance()->createOrGetTexture(str);
        if(tmp_tex)
        {
            result =tmp_tex;
        }
    }
    return result;
}

void Entity::createNode(Node *parent, tzw::CMC_Node * node)
{
    EntityNode * subEntity = new EntityNode(this);
    subEntity->setName (node->info ()->name ().c_str ());
    subEntity->setNodeData (node);
    parent->addChild (subEntity);
    if(parent == this)
    {
        m_skeleton->setEntityNodeRoot (subEntity);
    }
    for(int i =0; i< node->m_children.size ();i++)
    {
        createNode(subEntity,node->m_children[i]);
    }
}

float Entity::getPercentageOfAnimate()
{
    auto TimeInSeconds = m_animateTime;
    float TicksPerSecond =(float)(getModelData ()->animates ()[m_currentAnimateIndex]->m_ticksPerSecond != 0 ? getModelData ()->animates ()[m_currentAnimateIndex]->m_ticksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, getModelData ()->animates ()[m_currentAnimateIndex]->m_duration);
    return AnimationTime;
}

Skeleton *Entity::getSkeleton() const
{
    return m_skeleton;
}

void Entity::setSkeleton(Skeleton *skeleton)
{
    m_skeleton = skeleton;
}


int Entity::getCurrentAnimateIndex() const
{
    return m_currentAnimateIndex;
}

void Entity::setCurrentAnimateIndex(int currentAnimateIndex)
{
    m_currentAnimateIndex = currentAnimateIndex;
}

tzw::CMC_ModelData *Entity::getModelData() const
{
    return m_modelData;
}

void Entity::setModelData(tzw::CMC_ModelData *modelData)
{
    m_modelData = modelData;
}

