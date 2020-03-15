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
		void setLimit(float low, float high, float _softness = 0.9f, float _biasFactor = 0.3f, float _relaxationFactor = 1.0f);
		float getHingeAngle();
	protected:
		btHingeConstraint * m_constraint;
	};
}
