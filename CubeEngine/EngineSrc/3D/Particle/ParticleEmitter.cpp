#include "ParticleEmitter.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "Particle.h"
#include "Utility/math/TbaseMath.h"
#include <algorithm>
#include "3D/Primitive/CubePrimitive.h"

namespace tzw {
ParticleEmitter::ParticleEmitter(int maxSpawn)
  : m_spawnRate(0.1)
  , m_maxSpawn(maxSpawn)
  , m_spawnAmount(1)
  , m_currSpawn(0)
  , m_t(0)
  , m_state(State::Stop)
  , isLocalPos(false)
  , m_depthBias(0.0f)
  , m_isInfinite(true)
  , m_historyCount(0)
{

  auto mat = MaterialPool::shared()->getMaterialByName("Particle");

  if (!mat) {
    mat = Material::createFromTemplate("Particle");

    auto tex = TextureMgr::shared()->getByPath("Texture/glow.png");

    tex->genMipMap();

    mat->setTex("DiffuseMap", tex);
  }
  setMaterial(mat);

  setIsAccpectOcTtree(false);

  setCamera(g_GetCurrScene()->defaultCamera());

  initMesh();
}

void
ParticleEmitter::setUpTransFormation(TransformationInfo& info)
{
  auto currCam = g_GetCurrScene()->defaultCamera();
  info.m_projectMatrix = currCam->projection();
  info.m_viewMatrix = currCam->getViewMatrix();
  Matrix44 mat;
  mat.setToIdentity();
  info.m_worldMatrix = mat;
}

unsigned int
ParticleEmitter::getTypeId()
{
  return 2333;
}

void
ParticleEmitter::addInitModule(ParticleEmitterModule* module)
{
  m_initModule.push_back(module);
}

void
ParticleEmitter::addUpdateModule(ParticleEmitterModule* module)
{
  m_updateModule.push_back(module);
}

void
ParticleEmitter::logicUpdate(float dt)
{
  m_t += dt;
  if (m_state == State::Playing) {
    // spawn
    if (m_t > m_spawnRate) {
      if (m_currSpawn < m_maxSpawn) {
        int count = std::min(m_maxSpawn - m_currSpawn, m_spawnAmount);
        for (int i = 0; i < count; i++) {
          auto* p = new Particle();
          if (!isLocalPos) {
            p->m_pos = getTransform().transformVec3(vec3(0, 0, 0));
          } else {
            p->m_pos = vec3(0, 0, 0);
          }

          for (auto m : m_initModule) {
            m->process(p, this);
          }
          particleList.push_back(p);
          m_currSpawn += 1;
          m_historyCount += 1;
        }
      } else {
      }
      m_t = 0;
    }

    if (m_historyCount >= m_maxSpawn && !m_isInfinite) {
      m_state = State::Stop;
    }
  }

  if (m_state == State::Playing || m_state == State::Stop) {
    // logic update
    for (auto i = particleList.begin(); i != particleList.end();) {
      for (auto m : m_updateModule) {
        m->process(*i, this);
      }
      (*i)->step(dt);
      if ((*i)->isDead()) {
        // delete (*i);
        auto p = (*i);
        i = particleList.erase(i);
        delete p;
        m_currSpawn -= 1;
      } else {
        ++i;
      }
    }
  }
}

void
ParticleEmitter::pushCommand()
{}

void
ParticleEmitter::submitDrawCmd(RenderFlag::RenderStage passType)
{
  if (passType == RenderFlag::RenderStage::SHADOW) {
    return;
  }
  m_mesh->clearInstances();
  for (auto p : particleList) {
    InstanceData instance;
    vec3 targetPos;
    if (isLocalPos) 
	{
		targetPos = p->m_pos + getTransform().transformVec3(vec3(0, 0, 0));
    } else 
	{
		targetPos = p->m_pos;
    }
    instance.extraInfo = p->m_color;
  	reCache();
  	auto mat = getTransform();
    mat.setTranslate(vec3(targetPos));
    mat.setScale(vec3(p->size * p->m_initSize));
  	instance.transform = mat;//vec4(q.x, q.y, q.z, q.w);
    m_mesh->pushInstance(instance);
  }

  auto indexBuf = m_mesh->getIndexBuf();
  if (indexBuf->bufferId() <= 0) {
    m_mesh->submitOnlyVO_IO();
    m_mesh->submitInstanced(m_maxSpawn);
  } else {
    m_mesh->reSubmitInstanced();
  }
  if (m_mesh->getInstanceSize() > 0) {
    reCache();
    RenderCommand command(
      m_mesh, getMaterial(),this, RenderFlag::RenderStage::COMMON, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
    setUpTransFormation(command.m_transInfo);
    command.setRenderState(RenderFlag::RenderStage::TRANSPARENT);
    Renderer::shared()->addRenderCommand(command);
    setUpTransFormation(command.m_transInfo);
  }
}
void
ParticleEmitter::initMesh()
{
  m_mesh = new Mesh();
  float halfWidth = 0.8;
  float halfHeight = 0.8;
  VertexData vertices[] = {
    //#1
    VertexData(vec3(-1.0f * halfWidth, -1.0f * halfHeight, 0.0f),
               vec3(0, 0, 1),
               vec2(0.0f, 0.0f)),
    VertexData(vec3(1.0f * halfWidth, -1.0f * halfHeight, 0.0f),
               vec3(0, 0, 1),
               vec2(1.0f, 0.0f)),
    VertexData(vec3(1.0f * halfWidth, 1.0f * halfHeight, 0.0f),
               vec3(0, 0, 1),
               vec2(1.0f, 1.0f)),
    VertexData(vec3(-1.0f * halfWidth, 1.0f * halfHeight, 0.0f),
               vec3(0, 0, 1),
               vec2(0.0f, 1.0f)),
  };

  unsigned short indices[] = {
    0, 1, 2, 0, 2, 3,
  };
  m_mesh->addVertices(vertices, sizeof(vertices) / sizeof(VertexData));
  m_mesh->addIndices(indices, sizeof(indices) / sizeof(unsigned short));
  m_mesh->finish(false); // don't store vbo
  reCache();

  m_localAABB.merge(m_mesh->getAabb());
  reCache();
  reCacheAABB();
}

void
ParticleEmitter::setIsState(State state)
{
  m_state = state;
}

void
ParticleEmitter::setSpawnRate(float newSpawnRate)
{
  m_spawnRate = newSpawnRate;
  m_t = m_spawnRate;
}

float
ParticleEmitter::getSpawnRate() const
{
  return m_spawnRate;
}

int
ParticleEmitter::getSpawnAmount() const
{
  return m_spawnAmount;
}

void
ParticleEmitter::setTex(std::string filePath)
{
  auto mat = getMaterial();
  if (!mat) {
    mat = Material::createFromTemplate("Particle");
  }
  auto tex = TextureMgr::shared()->getByPath(filePath);

  tex->genMipMap();

  mat->setTex("DiffuseMap", tex);
}

void
ParticleEmitter::setSpawnAmount(const int spawnAmount)
{
  m_spawnAmount = spawnAmount;
}

float
ParticleEmitter::getDepthBias() const
{
  return m_depthBias;
}

void
ParticleEmitter::setBlendState(int state)
{
  auto mat = getMaterial();
  switch (state) {
    case 0:
      mat->setFactorDst(RenderFlag::BlendingFactor::One);
      mat->setFactorSrc(RenderFlag::BlendingFactor::SrcAlpha);
      break;
    case 1:
   		mat->setFactorDst(RenderFlag::BlendingFactor::OneMinusSrcAlpha);
  		mat->setFactorSrc(RenderFlag::BlendingFactor::SrcAlpha);
  	break;
  }
}

void
ParticleEmitter::setDepthBias(const float depthBias)
{
  m_depthBias = depthBias;
  auto mat = getMaterial();
  mat->setVar("TU_depthBias", depthBias);
}

void ParticleEmitter::setBillBoardPolicy(BillboardPolicy policy)
{
	switch (policy)
	{
	case BillboardPolicy::ALL:


	break;
	case BillboardPolicy::Y_FIXED:
		{
			auto mat = Material::createFromTemplate("ParticleFixedY");
			mat->setTex("DiffuseMap", getMaterial()->getTex("DiffuseMap"));
			setMaterial(mat);
		}
	break;
	default: ;
	}
}

ParticleEmitter::State ParticleEmitter::getState()
{
	return m_state;
}

bool
ParticleEmitter::isIsInfinite() const
{
  return m_isInfinite;
}

void
ParticleEmitter::setIsInfinite(const bool isInfinite)
{
  m_isInfinite = isInfinite;
}

bool
ParticleEmitter::isIsLocalPos() const
{
  return isLocalPos;
}

void
ParticleEmitter::setIsLocalPos(const bool isLocalPos)
{
  this->isLocalPos = isLocalPos;
}
} // namespace tzw
