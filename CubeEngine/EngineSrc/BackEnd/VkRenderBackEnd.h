#pragma once
#define NOMINMAX 1
#include "../Rendering/RenderFlag.h"
#include "../Engine/EngineDef.h"
#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include "vulkan/vk_sdk_platform.h"



#include <vector>
#include "RenderBackEndBase.h"
#include "vk/DevicePipelineVK.h"

#include <unordered_map>
#include "Rendering/RenderCommand.h"
class GLFW_BackEnd;
#define DEMO_TEXTURE_COUNT 1
namespace tzw {

class DeviceTextureVK;
class DeviceBufferVK;
class DeviceShaderVK;

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
struct RenderItem{
    VkDescriptorSet m_descriptorSet;
    VkBuffer m_uniformBuffer;
    VkDeviceMemory m_uniformBuffereMemory;
    Material * m_mat;
    TransformationInfo matrixInfo;
    Mesh * m_mesh;
    void updateDescriptor();
};
struct RenderItemPool
{
    RenderItemPool(Material * mat);
    RenderItem* findOrCreateRenderItem(Material * mat, void *obj);
    std::unordered_map<void *, RenderItem*> m_pool;
    VkDescriptorSetLayout m_layout;
    Material * m_mat;
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
    virtual DeviceTexture * loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag);
    virtual DeviceShader * createShader_imp();
    DeviceBuffer * createBuffer_imp() override;
    VkDevice getDevice();
    VkDescriptorPool & getDescriptorPool();


//helper
    bool memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    const VkSurfaceFormatKHR& GetSurfaceFormat() const;
    const VkSurfaceCapabilitiesKHR GetSurfaceCaps() const;
    const VkPhysicalDevice& GetPhysDevice();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView VKRenderBackEnd::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
    void createVKBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer&buffer, VkDeviceMemory& bufferMemory);
private:
    void VulkanEnumExtProps(std::vector<VkExtensionProperties>& ExtProps);
    void CreateInstance();
    void VulkanGetPhysicalDevices(const VkInstance& inst, const VkSurfaceKHR& Surface, VulkanPhysicalDevices& PhysDevices);
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void createSwapChain();
    void CreateCommandBuffer();
    void RecordCommandBuffers();
    void updateRecordCmdBuff(int swapImageIndex, int index, RenderItem * item, DevicePipelineVK * pipeline);

    void CreateRenderPass();
    void CreateFramebuffer();
    void CreateShaders();
    void CreatePipeline();
    void CreateUiniform();
    void CreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDecription);

    void CreateVertexBuffer();
    void updateUniformBuffer(uint32_t currentImage);
    void CreateIndexBuffer();
    void CreateDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void createDepthResources();

    void createCommandBufferForGeneral(int size);
    void createCommandBufferPoolForGeneral();
	
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void VKRenderBackEnd::createSyncObjects();
    void initImguiStuff();
	//depth resource
	VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
	std::vector<uint16_t> indices;
    VkInstance m_inst;
    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapChainKHR;
    VkQueue m_queue;
    VulkanPhysicalDevices m_physicsDevices;
    std::vector<VkCommandBuffer> m_cmdBufs;
    std::vector<std::vector<VkCommandBuffer>> m_generalCmdBuff;
    VkCommandPool m_cmdBufPool;
    VkCommandPool m_generalCmdBufPool;
    std::vector<VkImage> m_images;
    VkDevice m_device;
    unsigned m_gfxDevIndex;
    unsigned m_gfxQueueFamily;
    std::vector<VkImageView> m_views;	
    VkRenderPass m_renderPass;
    std::vector<VkFramebuffer> m_fbs;
    VkShaderModule m_vsModule;
    VkShaderModule m_fsModule;
    DeviceShaderVK *m_shader;
    VkPipeline m_pipeline;
    DeviceBufferVK * m_vertexBuffer;
    DeviceBufferVK * m_indexBuffer;
    //VkBuffer vertexBuffer;
    //VkBuffer indexBuffer;
    std::vector<VkBuffer> uniformBuffers;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPipelineLayout pipelineLayout;
    /*
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
    */
	VkSampler textureSampler;
    DeviceTextureVK * m_myTexture;
    //sync
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDebugReportCallbackEXT callback;
    //imgui
    DeviceBufferVK *m_imguiIndex;
    DeviceBufferVK *m_imguiVertex;
    Material * m_imguiMat;
    DevicePipelineVK * m_imguiPipeline;
    VkDescriptorSet m_imguiDescriptorSet;
    DeviceBufferVK * m_imguiUniformBuffer;
    //renderer
    std::unordered_map<std::string, DevicePipelineVK *>m_matPipelinePool;

    std::unordered_map<std::string, RenderItemPool*> m_matDescriptorSetPool;
};

} // namespace tzw

