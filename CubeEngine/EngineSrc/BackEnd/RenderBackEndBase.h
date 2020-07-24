#pragma once
#include "../Rendering/RenderFlag.h"
#include <string>
class GLFWwindow;
namespace tzw {

class RenderBackEndBase
{
public:
	virtual void initDevice(GLFWwindow * window);
}; // namespace tzw
}