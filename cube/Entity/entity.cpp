#include "entity.h"
#include <iostream>
#include "geometry/mesh.h"
#include "utility.h"
#include "string.h"
#include "base/bonedata.h"
#include "material/materialpool.h"
#include "texture/texturepool.h"
#include "shader/shader_program.h"
#include <QDebug>
Entity::Entity()
{
    this->m_pScene = NULL;
    this->mesh_list.clear();

    this->m_numBones = 0;
    this->m_animateTime = 0;
    this->m_hasAnimation = 0;
    this->setIsEnableShadow(true);
    this->onRender = NULL;
    this->setNodeType (NODE_TYPE_ENTITY);
    this->setShaderProgram (ShaderPoll::getInstance ()->get ("default"));
}

Entity::Entity(const char *file_name)
{
    this->mesh_list.clear();
    this->m_numBones = 0;
    this->m_animateTime = 0;
    this->setIsEnableShadow(true);
    this->onRender = NULL;
    this->setShaderProgram (ShaderPoll::getInstance ()->get ("default"));
    this->setNodeType (NODE_TYPE_ENTITY);
    loadModelData(file_name);
}

void Entity::addMesh(TMesh *mesh)
{
    this->mesh_list.push_back(mesh);
}

TMesh *Entity::getMesh(int index)
{
    return mesh_list[index];
}

void Entity::draw()
{
    for(int i=0;i<mesh_list.size();i++)
    {
        TMesh * mesh =mesh_list[i];
        MeshDrawComand * command = mesh->getCommand();
        command->synchronizeData(program,mesh->getMaterial(),mesh->getVerticesVbo(),mesh->getIndicesVbo());
        command->CommandSetRenderState(NULL,NULL,-1,-1);
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

void Entity::bonesTransform(float TimeInSeconds, std::vector<Matrix4f> &Transforms,std::string animation_name)
{
    if(!m_pScene  || m_pScene->mNumAnimations<=0)
    {
        Transforms.clear();
        return;
    }
    Matrix4f Identity;
    Identity.InitIdentity();
    float TicksPerSecond =(float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
    readNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);
    Transforms.resize(m_numBones);
    for (uint i = 0 ; i < m_numBones ; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

void Entity::animate(float time, const char *animation_name)
{

}
float Entity::animateTime() const
{
    return m_animateTime;
}

void Entity::setAnimateTime(float animateTime)
{
    m_animateTime = animateTime;
}
std::string Entity::animationName() const
{
    return m_animationName;
}

void Entity::setAnimationName(const std::string &animationName)
{
    m_animationName = animationName;
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

uint Entity::findBoneInterpoScaling(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    assert(0);

    return 0;
}

uint Entity::findBoneInterpoRotation(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

uint Entity::findBoneInterpoTranslation(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    assert(0);

    return 0;
}

const aiNodeAnim *Entity::findNodeAnim(const aiAnimation *pAnimation, const std::string NodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    return NULL;
}

void Entity::readNodeHeirarchy(float AnimationTime, const aiNode *pNode, const Matrix4f &ParentTransform)
{
    string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    Matrix4f NodeTransformation(pNode->mTransformation);
    const aiNodeAnim * pNodeAnim = findNodeAnim(pAnimation, NodeName);
    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_globalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void Entity::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = findBoneInterpoScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void Entity::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = findBoneInterpoRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void Entity::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = findBoneInterpoTranslation(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void Entity::loadModelData(const char *file_name)
{
    const aiScene* pScene = m_Importer.ReadFile(file_name,aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |aiProcess_CalcTangentSpace);
    this->m_pScene = (aiScene*)pScene;
    m_globalInverseTransform = pScene->mRootNode->mTransformation;
    m_globalInverseTransform = m_globalInverseTransform.Inverse();
    char str[100]={'\0'};
    utility::FindPrefix(file_name,str);
    LoadMaterial(pScene,file_name,str);
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for(int i =0 ;i< pScene->mNumMeshes ;i++)
    {
        const aiMesh* the_mesh = pScene->mMeshes[i];
        TMesh * mesh =new TMesh();
        this->addMesh(mesh);
        //set material
        mesh->setMaterial(this->material_list[the_mesh->mMaterialIndex]);
        for(int j =0; j<the_mesh->mNumVertices;j++)
        {
            const aiVector3D* pPos = &(the_mesh->mVertices[j]);
            const aiVector3D* pNormal = &(the_mesh->mNormals[j]);
            const aiVector3D* pTexCoord = the_mesh->HasTextureCoords(0) ? &(the_mesh->mTextureCoords[0][j]) : &Zero3D;
            const aiVector3D* pTangent = &(the_mesh->mTangents[i]);
            VertexData vec;
            vec.position = QVector3D(pPos->x,pPos->y,pPos->z);
            vec.normalLine = QVector3D(pNormal->x,pNormal->y,pNormal->z);
            vec.texCoord = QVector2D(pTexCoord->x,pTexCoord->y);
            vec.tangent = QVector3D(pTangent->x,pTangent->y,pTangent->z);
            mesh->pushVertex(vec);
        }

        for (unsigned int k = 0 ; k < the_mesh->mNumFaces ; k++) {
            const aiFace& Face = the_mesh->mFaces[k];
            assert(Face.mNumIndices == 3);
            mesh->pushIndex(Face.mIndices[0]);
            mesh->pushIndex(Face.mIndices[1]);
            mesh->pushIndex(Face.mIndices[2]);
        }
        //load bones
        loadBones(the_mesh,mesh);
        mesh->finish();
    }

}

void Entity::LoadMaterial(const aiScene *pScene, const char * file_name, const char *pre_fix)
{
    //store material
    for(int i = 0 ;i<pScene->mNumMaterials;i++)
    {
        aiColor3D dif(0.f,0.f,0.f);
        aiColor3D amb(0.f,0.f,0.f);
        aiColor3D spec(0.f,0.f,0.f);
        aiMaterial * the_material = pScene->mMaterials[i];
        char file_postfix[100];
        sprintf(file_postfix,"%s_%d",file_name,i+1);
        Material * material = MaterialPool::getInstance()->createMaterial(file_postfix);
        MaterialChannel * ambient_channel =  material->getAmbient();
        MaterialChannel * diffuse_channel =  material->getDiffuse();
        MaterialChannel * specular_channel =  material->getSpecular();
        the_material->Get(AI_MATKEY_COLOR_AMBIENT,amb);
        the_material->Get(AI_MATKEY_COLOR_DIFFUSE,dif);
        the_material->Get(AI_MATKEY_COLOR_SPECULAR,spec);

        ambient_channel->color = QVector3D(amb.r,amb.g,amb.b);
        diffuse_channel->color = QVector3D(dif.r,dif.g,dif.b);
        specular_channel->color = QVector3D(spec.r,spec.g,spec.b);
        aiString normalPath;
        the_material->GetTexture (aiTextureType_NORMALS,0,&normalPath);
        //now ,we only use the first diffuse texture, will fix it later
        aiString diffuse_Path;
        the_material->GetTexture(aiTextureType_DIFFUSE,0,&diffuse_Path);
        if(diffuse_Path.length == 0 )
        {
            diffuse_channel->texture = TexturePool::getInstance()->createTexture("default");
        }else
        {
            char str[100];
            Texture * tmp_tex = TexturePool::getInstance()->createTexture(diffuse_Path.C_Str());
            if(tmp_tex)
            {
                diffuse_channel->texture =tmp_tex;
                goto END;
            }
            sprintf(str,"%s%s",pre_fix,diffuse_Path.C_Str());
            tmp_tex = TexturePool::getInstance()->createTexture(str);
            if(tmp_tex)
            {
                diffuse_channel->texture =tmp_tex;
                goto END;
            }
            sprintf(str,"%s%s","res/texture/",diffuse_Path.C_Str());
            tmp_tex = TexturePool::getInstance()->createTexture(str);
            if(tmp_tex)
            {
                diffuse_channel->texture =tmp_tex;
                goto END;
            }

        }
END:
        this->material_list.push_back(material);
    }
}

void Entity::loadBones( const aiMesh *pMesh,TMesh * mesh)
{
    this->m_hasAnimation = m_pScene->HasAnimations();
    if(!m_hasAnimation)
    {
        return;
    }
    vector<BoneData> Bones;
    Bones.resize(pMesh->mNumVertices);
    for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        string BoneName(pMesh->mBones[i]->mName.data);
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = m_numBones;
            m_numBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }
        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
            uint VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].addBoneData(BoneIndex, Weight);
        }
    }
    mesh->setBones(Bones);
}
