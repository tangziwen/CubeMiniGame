#pragma once
class btTypedConstraint;
namespace tzw
{
	class PhysicsConstraint
	{
	public:
		virtual ~PhysicsConstraint();
		explicit PhysicsConstraint();
		virtual btTypedConstraint* constraint() const;
		virtual void setConstraint(btTypedConstraint* constraint);
	protected:
		btTypedConstraint * m_constraint;
	};
}
