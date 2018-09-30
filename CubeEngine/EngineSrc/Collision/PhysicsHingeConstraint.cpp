#include "PhysicsHingeConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

namespace tzw
{
	PhysicsHingeConstraint::PhysicsHingeConstraint(btHingeConstraint * constraint)
		: m_constraint(constraint)
	{
	}

btHingeConstraint* PhysicsHingeConstraint::constraint() const
{
	return m_constraint;
}

void PhysicsHingeConstraint::setConstraint(btHingeConstraint* constraint)
{
	m_constraint = constraint;
}

void PhysicsHingeConstraint::enableAngularMotor(bool enableMotor, float targetVelocity, float maxMotorImpulse)
{
	m_constraint->enableAngularMotor(enableMotor, targetVelocity, maxMotorImpulse);
}

}
