#include "DrawPass.h"

namespace tzw
{
DrawPassType parseDrawPassType(const std::string& name, DrawPassType defaultType)
{
	if(name == "GBuffer" || name == "GBUFFER" || name == "COMMON")
	{
		return DrawPassType::GBuffer;
	}
	if(name == "Transparent" || name == "TRANSPARENT")
	{
		return DrawPassType::Transparent;
	}
	if(name == "AfterDepthClear" || name == "AFTER_DEPTH_CLEAR")
	{
		return DrawPassType::AfterDepthClear;
	}
	if(name == "GUI")
	{
		return DrawPassType::GUI;
	}
	if(name == "Shadow" || name == "SHADOW")
	{
		return DrawPassType::Shadow;
	}
	if(name == "DebugLayer" || name == "DEBUG_LAYER")
	{
		return DrawPassType::DebugLayer;
	}
	if(name == "OutlineMask" || name == "OUTLINE_MASK")
	{
		return DrawPassType::OutlineMask;
	}
	return defaultType;
}

const char* drawPassTypeToString(DrawPassType type)
{
	switch(type)
	{
	case DrawPassType::Unset:
		return "Unset";
	case DrawPassType::GBuffer:
		return "GBuffer";
	case DrawPassType::Transparent:
		return "Transparent";
	case DrawPassType::AfterDepthClear:
		return "AfterDepthClear";
	case DrawPassType::GUI:
		return "GUI";
	case DrawPassType::Shadow:
		return "Shadow";
	case DrawPassType::DebugLayer:
		return "DebugLayer";
	case DrawPassType::OutlineMask:
		return "OutlineMask";
	case DrawPassType::All:
		return "All";
	default:
		return "Unknown";
	}
}
}
