
#include "PhysicsConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

namespace tzw
{
PhysicsConstraint::PhysicsConstraint()
	: m_constraint(nullptr)
{

}

btTypedConstraint* PhysicsConstraint::constraint() const
{
	return m_constraint;
}

void PhysicsConstraint::setConstraint(btTypedConstraint* constraint)
{
	m_constraint = constraint;
}


}
