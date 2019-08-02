#pragma once
#include "PhysicsConstraint.h"
class btHingeConstraint;
class btTypedConstraint;
namespace tzw
{
	class PhysicsHingeConstraint : public PhysicsConstraint
	{
	public:

		explicit PhysicsHingeConstraint(btHingeConstraint* constraint);
		btTypedConstraint* constraint() const override;
		void setConstraint(btTypedConstraint* constraint) override;
		void enableAngularMotor(bool enableMotor,float targetVelocity, float maxMotorImpulse);
	protected:
		btHingeConstraint * m_constraint;
	};
}
