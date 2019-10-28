#include "ParticleEmitter.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Particle.h"
#include <algorithm>

namespace tzw {
ParticleEmitter::ParticleEmitter(): m_spawnRate(0.2), m_maxSpawn(20), m_spawnAmount(2), m_currSpawn(0), m_t(0)
{

	auto mat = MaterialPool::shared()->getMaterialByName("grassMaterial");
	 
	if (!mat)
	{

		mat = Material::createFromTemplate("Grass");
		 
		auto tex = TextureMgr::shared()->getByPath("Texture/grass.tga");
		 
		tex->genMipMap();
		 
		mat->setTex("diffuseMap", tex);
		 
	}
	setMaterial(mat);
	 
	setIsAccpectOcTtree(false);
	 
	setCamera(g_GetCurrScene()->defaultCamera());

	initMesh();
}


void ParticleEmitter::setUpTransFormation(TransformationInfo &info)
{
	auto currCam = g_GetCurrScene()->defaultCamera();
	info.m_projectMatrix = currCam->projection();
	info.m_viewMatrix = currCam->getViewMatrix();
	info.m_worldMatrix = getTransform();
}

unsigned int ParticleEmitter::getTypeId()
{
	return 2333;
}

void ParticleEmitter::addInitModule(ParticleEmitterModule module)
{
	m_initModule.push_back(module);
}

void ParticleEmitter::addUpdateModule(ParticleEmitterModule module)
{
	m_updateModule.push_back(module);
}

void ParticleEmitter::logicUpdate(float dt)
{
	m_t += dt;
	//spawn
	if(m_t > m_spawnRate && m_currSpawn < m_maxSpawn)
	{
		int count = std::min(m_maxSpawn - m_currSpawn, m_spawnAmount);
		for(int i = 0; i < m_spawnAmount; i++)
		{
			auto * p = new Particle();
			for(auto & m : m_initModule)
			{
				m.process(p);
			}
			particleList.push_back(p);
		}
		m_t = 0;
	}

	//logic update
	for(auto i = particleList.begin(); i != particleList.end(); )
	{
		for(auto & m : m_updateModule)
		{
			m.process(*i);
		}
		(*i)->step(dt);
		//if((*i)->isDead())
		//{
		//	delete (*i);
		//	i = particleList.erase(i);
		//}else
		{
			++i;
		}
	}
}

void ParticleEmitter::pushCommand()
{

}

void ParticleEmitter::submitDrawCmd(RenderCommand::RenderType passType)
{
	m_mesh->clearInstances();
	for(auto p: particleList) 
	{
      	InstanceData instance;
      	instance.posAndScale = vec4(p->m_pos, 1.0);
      	instance.extraInfo = vec4(1, 0, 1, 0);
        m_mesh->pushInstance(instance);
	}
	if(m_mesh->getIndexBuf()->bufferId() <= 0)
	{
		m_mesh->submitOnlyVO_IO();
		m_mesh->submitInstanced(m_maxSpawn);
	}else
	{
		m_mesh->reSubmitInstanced();
	}
	
	RenderCommand command(m_mesh, getMaterial(), RenderCommand::RenderType::Instanced);
	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
	setUpTransFormation(command.m_transInfo);
}
void ParticleEmitter::initMesh()
{
	m_mesh = new Mesh();
	float halfWidth = 0.8;
	float halfHeight = 0.8;
	VertexData vertices[] = {
		//#1
		VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -0.5f), vec2(0.0f, 0.0f)), 
		VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  -0.5f), vec2(1.0f, 0.0f)),
		VertexData(vec3(1.0f *halfWidth,  1.0f * halfHeight,  -0.5f), vec2(1.0f, 1.0f)), 
		VertexData(vec3( -1.0f *halfWidth,  1.0f * halfHeight,  -0.5f), vec2(0.0f, 1.0f)),

		//#2
		VertexData(vec3(-0.707f *halfWidth - 0.353, -1.0f * halfHeight, -0.707f *halfWidth + 0.353), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.707f *halfWidth - 0.353, 1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(1.0f, 1.0f)),
		VertexData(vec3(-0.707f *halfWidth - 0.353,  1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(0.0f, 1.0f)),
		VertexData(vec3(0.707f *halfWidth - 0.353,  -1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(1.0f, 0.0f)),

		//#3
		VertexData(vec3(-0.707f *halfWidth + 0.353, -1.0f * halfHeight, 0.707f *halfWidth + 0.353), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.707f *halfWidth + 0.353, 1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(1.0f, 1.0f)),
		VertexData(vec3(-0.707f *halfWidth + 0.353,  1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(0.0f, 1.0f)),
		VertexData(vec3(0.707f *halfWidth + 0.353,  -1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(1.0f, 0.0f)),

	};

	unsigned short indices[] = {
		0, 1, 2, 0, 2, 3,     4, 5, 6, 4, 7, 5,   8, 9, 10, 8, 11, 9,
	};
	m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
	m_mesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
	m_mesh->finish(false);//don't store vbo
	reCache();

	m_localAABB.merge(m_mesh->getAabb());
	reCache();
	reCacheAABB();
}
} // namespace tzw
