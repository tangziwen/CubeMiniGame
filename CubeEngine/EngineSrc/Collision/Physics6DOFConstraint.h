#pragma once
#include "PhysicsConstraint.h"
class btGeneric6DofConstraint;
class btTypedConstraint;
namespace tzw
{
	class vec3;

	class Physics6DOFConstraint : public PhysicsConstraint
	{
	public:

		explicit Physics6DOFConstraint(btGeneric6DofConstraint* constraint);
		btTypedConstraint* constraint() const;
		void setConstraint(btTypedConstraint* constraint);
		void setLinearUpperLimit(vec3 linearHigh);
		void setLinearLowerLimit(vec3 linearLow);

		void setAngularUpperLimit(vec3 angularHigh);
		void setAngularLowerLimit(vec3 angularLow);
		void makeUpBearing();
	protected:
		btGeneric6DofConstraint * m_constraint;
	};
}
