#include "Physics6DOFConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "Math/vec3.h"
#include "Bullet3Common/b3Transform.h"
#include "Bullet3Collision/NarrowPhaseCollision/shared/b3RigidBodyData.h"

namespace tzw
{
	Physics6DOFConstraint::Physics6DOFConstraint(btGeneric6DofConstraint* constraint)
	{
		m_constraint = constraint;
	}

	btTypedConstraint* Physics6DOFConstraint::constraint() const
	{
		return m_constraint;
	}

	void Physics6DOFConstraint::setConstraint(btTypedConstraint* constraint)
	{
		m_constraint = static_cast<btGeneric6DofConstraint *>(constraint);
	}

	void Physics6DOFConstraint::setLinearUpperLimit(vec3 linearHigh)
	{
		m_constraint->setLinearUpperLimit(btVector3(linearHigh.x, linearHigh.y, linearHigh.z));
	}

	void Physics6DOFConstraint::setLinearLowerLimit(vec3 linearLow)
	{
		m_constraint->setLinearLowerLimit(btVector3(linearLow.x, linearLow.y, linearLow.z));
	}

	void Physics6DOFConstraint::setAngularUpperLimit(vec3 angularHigh)
	{
		m_constraint->setAngularUpperLimit(btVector3(angularHigh.x, angularHigh.y, angularHigh.z));
	}

	void Physics6DOFConstraint::setAngularLowerLimit(vec3 angularLow)
	{
		m_constraint->setAngularLowerLimit(btVector3(angularLow.x, angularLow.y, angularLow.z));
	}
	static b3Transform getCenterOfMassTransform(const b3RigidBodyData& body)
	{
		b3Transform tr(body.m_quat, body.m_pos);
		return tr;
	}
	void Physics6DOFConstraint::makeUpBearing()
	{
		auto transA = m_constraint->getRigidBodyA().getCenterOfMassTransform();
		auto transB = m_constraint->getRigidBodyA().getCenterOfMassTransform();
		m_constraint->setAngularLowerLimit(btVector3(0,0, -999));
		m_constraint->setAngularUpperLimit(btVector3(0,0, 999));
		m_constraint->setLinearLowerLimit(btVector3(0,0, 0));
		m_constraint->setLinearUpperLimit(btVector3(0,0, 0));
		m_constraint->setBreakingImpulseThreshold(9999999999999999);;

		m_constraint->setOverrideNumSolverIterations(240);
	}
}
