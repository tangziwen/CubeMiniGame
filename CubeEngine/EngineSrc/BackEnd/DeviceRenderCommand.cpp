#include "DeviceRenderCommand.h"

namespace tzw
{
DeviceRenderCommand::DeviceRenderCommand(void* handle):m_handle(handle)
{
	
}


void* DeviceRenderCommand::get()
{
	return m_handle;
}

}
