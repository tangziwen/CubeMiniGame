#include "Physics6DOFSprintConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.h"
#include "Math/vec3.h"

namespace tzw
{
	Physics6DofConstraint::Physics6DofConstraint(btGeneric6DofSpring2Constraint* constraint)
	{
		m_constraint = constraint;
	}

	btTypedConstraint* Physics6DofConstraint::constraint() const
	{
		return m_constraint;
	}

	void Physics6DofConstraint::setConstraint(btTypedConstraint* constraint)
	{
		m_constraint = static_cast<btGeneric6DofSpring2Constraint *>(constraint);
	}

	void Physics6DofConstraint::setLinearUpperLimit(vec3 linearHigh)
	{
		m_constraint->setLinearUpperLimit(btVector3(linearHigh.x, linearHigh.y, linearHigh.z));
	}

	void Physics6DofConstraint::setLinearLowerLimit(vec3 linearLow)
	{
		m_constraint->setLinearLowerLimit(btVector3(linearLow.x, linearLow.y, linearLow.z));
	}

	void Physics6DofConstraint::setAngularUpperLimit(vec3 angularHigh)
	{
		m_constraint->setAngularUpperLimit(btVector3(angularHigh.x, angularHigh.y, angularHigh.z));
	}

	void Physics6DofConstraint::setAngularLowerLimit(vec3 angularLow)
	{
		m_constraint->setAngularLowerLimit(btVector3(angularLow.x, angularLow.y, angularLow.z));
	}

	void Physics6DofConstraint::enableSpring(int index, bool isEnable)
	{
		m_constraint->enableSpring(index, isEnable);
	}

	void Physics6DofConstraint::setStiffness(int index, float setStiffness)
	{
		m_constraint->setStiffness(index, setStiffness);
	}

	void Physics6DofConstraint::setDamping(int index, float damping)
	{
			m_constraint->setDamping(index, damping);
	}

	void Physics6DofConstraint::setEquilibriumPoint()
	{
		m_constraint->setEquilibriumPoint();
	}
}
