#include "PhysicsHingeConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

namespace tzw
{
	PhysicsHingeConstraint::PhysicsHingeConstraint(btHingeConstraint * constraint)
		: m_constraint(constraint)
	{
	}

btTypedConstraint* PhysicsHingeConstraint::constraint() const
{
	return m_constraint;
}

void PhysicsHingeConstraint::setConstraint(btTypedConstraint* constraint)
{
	m_constraint = static_cast<btHingeConstraint *>(constraint);
}

void PhysicsHingeConstraint::enableAngularMotor(bool enableMotor, float targetVelocity, float maxMotorImpulse)
{
	//let the rigidBody wake up
	m_constraint->getRigidBodyA().activate();
	m_constraint->getRigidBodyB().activate();
	printf("aaaaaa%d, %d\n",m_constraint->getRigidBodyA().getActivationState(), m_constraint->getRigidBodyB().getActivationState());
	m_constraint->enableAngularMotor(enableMotor, targetVelocity, maxMotorImpulse);
}

}
