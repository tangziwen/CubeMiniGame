#pragma once
class btGeneric6DofSpringConstraint;
namespace tzw
{
	class vec3;

	class Physics6DofConstraint
	{
	public:

		explicit Physics6DofConstraint(btGeneric6DofSpringConstraint* constraint);
		btGeneric6DofSpringConstraint* constraint() const;
		void setConstraint(btGeneric6DofSpringConstraint* constraint);
		void setLinearUpperLimit(vec3 linearHigh);
		void setLinearLowerLimit(vec3 linearLow);

		void setAngularUpperLimit(vec3 angularHigh);
		void setAngularLowerLimit(vec3 angularLow);
		void enableSpring(int index, bool isEnable);
		void setStiffness(int index, float setStiffness);
		void setDamping(int index, float damping);
		void setEquilibriumPoint();
	protected:
		btGeneric6DofSpringConstraint * m_constraint;
	};
}
