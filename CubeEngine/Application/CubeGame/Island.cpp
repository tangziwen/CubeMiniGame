#include "Island.h"
#include "../../EngineSrc/Scene/SceneMgr.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "Collision/PhysicsRigidBody.h"

namespace tzw {
Island::Island(vec3 pos)
{
  // the toehold here
  m_node = new Drawable3D();
  m_node->setPos(pos);
  g_GetCurrScene()->addNode(m_node);
  m_rigid = nullptr;
  m_compound_shape = nullptr;
}

void
Island::insert(GamePart* part)
{
  if (!part->getNode()) {
    printf("shit\n");
  }
  m_partList.push_back(part);
  part->m_parent = this;

  // recalculate Princilpal axis

#if 0 // water code
		if(m_rigid)
		{   
			PhysicsMgr::shared()->removeRigidBody(m_rigid);
			delete m_rigid;
			m_rigid = nullptr;
			if(getCompoundShape()) 
			{
				recalculateCompound();
			}
			cook();
		}
#else // below should work, but in fact not, i don't know why (edit: after i add
      // activate, it works!)

  if (m_rigid) {
    PhysicsMgr::shared()->removeRigidBody(m_rigid);
    if (getCompoundShape()) {
      recalculateCompound();
      m_rigid->setMass(getMass(),
                       getCompoundShape()->calculateLocalInertia(getMass()));
      m_rigid->updateInertiaTensor();
      // recalculate will modify the transform position, though the final
      // islandMatrix * childMatrix will remain the same, we need to recalculate
      // the rigid-body position
      auto startTransform = m_node->getTransform();
      m_rigid->setWorldTransform(startTransform);
    }
    m_rigid->setCollisionShape(getCompoundShape());
    m_rigid->updateInertiaTensor();
    PhysicsMgr::shared()->addRigidBody(m_rigid);
    m_rigid->updateInertiaTensor();

    //没有activate是不会生效的，因为可能物体已经sleep了，所以会悬着
    m_rigid->activate();
  }
#endif
}

void
Island::remove(GamePart* part)
{
  auto result = std::find(m_partList.begin(), m_partList.end(), part);
  if (result != m_partList.end()) {
    m_partList.erase(result);
  }
  part->m_parent = this;
}

PhysicsCompoundShape*
Island::getCompoundShape() const
{
  return m_compound_shape;
}

void
Island::setCompoundShape(PhysicsCompoundShape* compoundShape)
{
  m_compound_shape = compoundShape;
}

void
Island::recalculateCompound()
{
  if (!getCompoundShape())
    return;
  for (auto part : m_partList) {
    if (part->getType() != GAME_PART_LIFT) {
      auto mat = part->getNode()->getLocalTransform();
      m_compound_shape->addChildShape(&mat, part->getShape()->getRawShape());
    } else {
    }
  }
  auto principleMat = m_compound_shape->adjustPrincipalAxis();
  for (auto part : m_partList) {
    if (part->getType() != GAME_PART_LIFT) {
      auto mat = part->getNode()->getLocalTransform();
      mat = principleMat.inverted() * mat;
      part->getNode()->setPos(mat.getTranslation());
      Quaternion q;
      q.fromRotationMatrix(&mat);
      part->getNode()->setRotateQ(q);
    }
  }
  auto mat = m_node->getLocalTransform();
  mat = principleMat * mat;
  m_node->setPos(mat.getTranslation());
  Quaternion q;
  q.fromRotationMatrix(&mat);
  m_node->setRotateQ(q);
  m_node->reCache();
}

void
Island::enablePhysics(bool isEnable)
{
	if(isEnable) 
	{
		if (!m_rigid) // has been created yet
		{
		  cook();
		}
		else 
		{
			PhysicsMgr::shared()->addRigidBody(m_rigid);
		}
	} else 
	{
        if(m_rigid) 
		{
			PhysicsMgr::shared()->removeRigidBody(m_rigid);
        }
	}
}

float
Island::getMass()
{
  float mass = 0.0f;
  for (auto part : m_partList) {
    if (part->getType() != GAME_PART_LIFT) {
      auto mat = part->getNode()->getLocalTransform();
      m_compound_shape->addChildShape(&mat, part->getShape()->getRawShape());
      mass += part->getMass();
    }
  }
  return mass;
}

void
Island::cook()
{
  PhysicsCompoundShape* compoundShape = getCompoundShape();
  if (!compoundShape) {
    compoundShape = new PhysicsCompoundShape();
    setCompoundShape(compoundShape);
  }
  recalculateCompound();
  auto partMat = m_node->getTransform();
  auto rig = PhysicsMgr::shared()->createRigidBodyFromCompund(
    1.0 * m_partList.size(), &partMat, compoundShape);
  rig->attach(m_node);
  m_rigid = rig;
}

void
Island::addNeighbor(Island* island)
{
  m_neighborIslands.insert(island);
}

void
Island::removeNeighbor(Island* island)
{
  auto result = m_neighborIslands.find(island);
  if (result != m_neighborIslands.end()) {
    m_neighborIslands.erase(result);
  }
}

const std::set<Island*>
Island::getNeighBor() const
{
  return m_neighborIslands;
}
}
