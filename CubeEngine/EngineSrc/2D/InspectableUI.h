#pragma once
#include <string>
#include "Math/vec4.h"
namespace tzw
{
class InspectableUI
{
public:
	virtual std::string getTitle();
	virtual void inspect();

	void bindFloat(std::string name, float * value, float min, float max, const char * fmt);
	void bindColor(std::string name, vec3 * color);
};
}
