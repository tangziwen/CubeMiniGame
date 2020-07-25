#pragma once

#include "../Rendering/RenderFlag.h"
#include "../Engine/EngineDef.h"
#include <string>
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include "vulkan/vk_sdk_platform.h"
#include <vector>
#include "RenderBackEndBase.h"
class GLFW_BackEnd;
#define DEMO_TEXTURE_COUNT 1
namespace tzw {

typedef struct {
    VkImage image;
    VkCommandBuffer cmd;
    VkImageView view;
} SwapchainBuffers;
struct texture_object {
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    VkDeviceMemory mem;
    VkImageView view;
    int32_t tex_width, tex_height;
};
struct VulkanPhysicalDevices {
    std::vector<VkPhysicalDevice> m_devices;
    std::vector<VkPhysicalDeviceProperties> m_devProps;
    std::vector< std::vector<VkQueueFamilyProperties> > m_qFamilyProps;
    std::vector< std::vector<VkBool32> > m_qSupportsPresent;
    std::vector< std::vector<VkSurfaceFormatKHR> > m_surfaceFormats;
    std::vector<VkSurfaceCapabilitiesKHR> m_surfaceCaps;
};


class VKRenderBackEnd:public Singleton<VKRenderBackEnd>, public RenderBackEndBase
{
public:
    VKRenderBackEnd();
    void initDevice(GLFWwindow * window) override;
    VkInstance & getVKInstance();
    void setVKSurface(const VkSurfaceKHR & surface);
    VkSurfaceKHR &getVKSurface();
    void initDevices();
    void RenderScene();
private:
    void VulkanEnumExtProps(std::vector<VkExtensionProperties>& ExtProps);
    void CreateInstance();
    void VulkanGetPhysicalDevices(const VkInstance& inst, const VkSurfaceKHR& Surface, VulkanPhysicalDevices& PhysDevices);
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void createSwapChain();
    void CreateCommandBuffer();
    void RecordCommandBuffers();

    void CreateRenderPass();
    void CreateFramebuffer();
    void CreateShaders();
    void CreatePipeline();
    void CreateUiniform();
    void CreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDecription);

    void CreateVertexBuffer();

    void CreateIndexBuffer();

    //helper
    bool memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
    const VkSurfaceFormatKHR& GetSurfaceFormat() const;
    const VkSurfaceCapabilitiesKHR GetSurfaceCaps() const;
    const VkPhysicalDevice& GetPhysDevice();
    VkInstance m_inst;
    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapChainKHR;
    VkQueue m_queue;
    VulkanPhysicalDevices m_physicsDevices;
    std::vector<VkCommandBuffer> m_cmdBufs;
    VkCommandPool m_cmdBufPool;
    std::vector<VkImage> m_images;
    VkDevice m_device;
    unsigned m_gfxDevIndex;
    unsigned m_gfxQueueFamily;
    std::vector<VkImageView> m_views;	
    VkRenderPass m_renderPass;
    std::vector<VkFramebuffer> m_fbs;
    VkShaderModule m_vsModule;
    VkShaderModule m_fsModule;
    VkPipeline m_pipeline;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkPhysicalDeviceMemoryProperties memory_properties;
};

} // namespace tzw

