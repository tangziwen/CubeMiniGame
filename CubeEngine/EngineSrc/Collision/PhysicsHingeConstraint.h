#pragma once
class btHingeConstraint;
namespace tzw
{
	class PhysicsHingeConstraint
	{
	public:

		explicit PhysicsHingeConstraint(btHingeConstraint* constraint);
		btHingeConstraint* constraint() const;
		void setConstraint(btHingeConstraint* constraint);
		void enableAngularMotor(bool enableMotor,float targetVelocity, float maxMotorImpulse);
	protected:
		btHingeConstraint * m_constraint;
	};
}
