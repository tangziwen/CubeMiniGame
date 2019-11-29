#pragma once
#include "PhysicsConstraint.h"
class btGeneric6DofSpring2Constraint;
class btTypedConstraint;
namespace tzw
{
	class vec3;

	class Physics6DofSpringConstraint : public PhysicsConstraint
	{
	public:

		explicit Physics6DofSpringConstraint(btGeneric6DofSpring2Constraint* constraint);
		btTypedConstraint* constraint() const;
		void setConstraint(btTypedConstraint* constraint);
		void setLinearUpperLimit(vec3 linearHigh);
		void setLinearLowerLimit(vec3 linearLow);

		void setAngularUpperLimit(vec3 angularHigh);
		void setAngularLowerLimit(vec3 angularLow);
		void enableSpring(int index, bool isEnable);
		void setStiffness(int index, float setStiffness);
		void setDamping(int index, float damping);
		void setEquilibriumPoint();
	protected:
		btGeneric6DofSpring2Constraint * m_constraint;
	};
}
