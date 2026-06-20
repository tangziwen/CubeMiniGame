#ifndef TZW_DEBUGPRIMITIVE_H
#define TZW_DEBUGPRIMITIVE_H

#include "Math/vec3.h"
#include <memory>

namespace tzw
{
class LinePrimitive;
class RenderQueue;

// DebugSystem owns DebugPrimitive instances returned by createXXX APIs.
class DebugPrimitive
{
public:
	DebugPrimitive();
	~DebugPrimitive();
	DebugPrimitive(const DebugPrimitive& other) = delete;
	DebugPrimitive& operator=(const DebugPrimitive& other) = delete;

	void setVisible(bool visible);
	bool isVisible() const;
	void setDefaultColor(vec3 color);
	vec3 getDefaultColor() const;
	void setColor(vec3 color);
	void requestDestroy();
	bool isDestroyRequested() const;

	void clear();
	void appendLine(vec3 begin, vec3 end);
	void appendLine(vec3 begin, vec3 end, vec3 color);
	int getLineCount() const;
	void submitDrawCmd(RenderQueue* queue, int requirementArg);
	LinePrimitive* getLinePrimitive();

private:
	std::unique_ptr<LinePrimitive> m_line;
	vec3 m_defaultColor;
	bool m_isVisible;
	bool m_destroyRequested;
};

} // namespace tzw

#endif // TZW_DEBUGPRIMITIVE_H
