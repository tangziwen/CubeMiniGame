#pragma once
#define NOMINMAX 1
#include "../Rendering/RenderFlag.h"
#include "../Engine/EngineDef.h"
#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include "vulkan/vk_sdk_platform.h"
#include "Rendering/RenderPath.h"

#include <vector>
#include "RenderBackEndBase.h"
#include "vk/DevicePipelineVK.h"

#include <unordered_map>
#include "Rendering/RenderCommand.h"
#include <EngineSrc\BackEnd\vk\DeviceItemBufferPoolVK.h>
#include "vk/DeviceMemoryPoolVK.h"
#include "vk/DeviceRenderPassVK.h"
#include "vk/DeviceFrameBufferVK.h"
#include <set>
#include "vk/DeviceRenderStageVK.h"
#include <unordered_set>
class GLFW_BackEnd;
#define DEMO_TEXTURE_COUNT 1
#define CHECK_VULKAN_ERROR(a, b) {if(b){printf(a, b);abort();}}
#define VK_CHECK_RESULT(func) {VkResult result = func; if(result !=VK_SUCCESS) {abort();}}
namespace tzw {

class DeviceTextureVK;
class DeviceBufferVK;
class DeviceShaderVK;
class SpherePrimitive;
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

struct FrameBufferAttachmentVK {
	VkImage image;
	VkDeviceMemory mem;
	VkImageView view;
	VkFormat format;
};
struct ItemUniform
{
    alignas(16) Matrix44 wvp;
    alignas(16) Matrix44 wv;
    alignas(16) Matrix44 world;
    alignas(16) Matrix44 view;
    alignas(16) Matrix44 projection;
};
struct FrameBufferVK{
	int32_t width, height;
	VkFramebuffer frameBuffer;
	FrameBufferAttachmentVK position, normal, albedo;
	FrameBufferAttachmentVK depth;
	VkRenderPass renderPass;
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
    DeviceTexture * loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag) override;
    DeviceTexture * loadTextureRaw_imp(const unsigned char* buf, int width, int height, ImageFormat byte_size, unsigned int loadingFlag) override;
    DeviceShader * createShader_imp() override;
    DeviceBuffer * createBuffer_imp() override;
    VkDevice getDevice();
    VkDescriptorPool & getDescriptorPool();
    DeviceItemBufferPoolVK * getItemBufferPool();

//helper
    bool memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, int baseMipLevel = 0, int levelCount = 1);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, int mipLevel = 0);
    const VkSurfaceFormatKHR& GetSurfaceFormat() const;
    const VkSurfaceCapabilitiesKHR GetSurfaceCaps() const;
    const VkPhysicalDevice& GetPhysDevice();
    const VulkanPhysicalDevices GetPhysDeviceWrapper();
    VkPhysicalDeviceProperties GetPhysDeviceProperties();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, int mipLevels = 1);
	VkImageView VKRenderBackEnd::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,int baseMipLevel = 0, int levelCount = 1);
    void createVKBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer&buffer, VkDeviceMemory& bufferMemory);
    DeviceMemoryPoolVK * getMemoryPool();
    VkFormat getFormat(ImageFormat imageFormat);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	virtual void prepareFrame();
	virtual void endFrame();
    VkCommandBuffer getGeneralCommandBuffer();
    void clearCommandBuffer();
    std::unordered_map<Material *, DevicePipelineVK *> & getPipelinePool();
    void updateItemDescriptor(VkDescriptorSet itemDescSet, Material * mat, size_t m_offset, size_t bufferRange);
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
    void CreateTextureToScreenRenderPass();
    void CreateDerrferredRenderPass();
    void createDeferredFrameBuffer();
    	// Create a frame buffer attachment
	void createAttachment(
		VkFormat format,
		VkImageUsageFlagBits usage,
		FrameBufferAttachmentVK *attachment);
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
	

    void VKRenderBackEnd::createSyncObjects();
    void initImguiStuff();

    void initVMAPool();

    void shadowPass();
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
    VkRenderPass m_textureToScreenRenderPass;
    VkRenderPass m_gbufferRenderPass;
    std::vector<VkFramebuffer> m_fbs;
    VkShaderModule m_vsModule;
    VkShaderModule m_fsModule;
    DeviceShaderVK *m_shader;
    VkPipeline m_pipeline;
    DeviceBufferVK * m_vertexBuffer;
    DeviceBufferVK * m_indexBuffer;

    DeviceBufferVK * m_quadVertexBuffer;
    DeviceBufferVK * m_quadIndexBuffer;
    //VkBuffer vertexBuffer;
    //VkBuffer indexBuffer;
    std::vector<VkBuffer> uniformBuffers;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPipelineLayout pipelineLayout;

    void drawObjs(VkCommandBuffer command, std::vector<RenderCommand>& renderList);
    void drawObjs_Common(std::unordered_map<Material *, DevicePipelineVK *> & pipelinepool, VkCommandBuffer command, DeviceRenderStageVK * renderStage , std::vector<RenderCommand>& renderList);

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
    Material * m_shadowMat;
    Material * m_shadowInstancedMat;
    DevicePipelineVK * m_imguiPipeline;
    VkDescriptorSet m_imguiDescriptorSet;
    DeviceBufferVK * m_imguiUniformBuffer;
    //renderer
    std::unordered_map<Material *, DevicePipelineVK *>m_matPipelinePool;
    std::unordered_map<Material *, DevicePipelineVK *>m_thumbNailPipelinePool;

    DeviceItemBufferPoolVK * m_itemBufferPool;
    DeviceMemoryPoolVK * m_memoryPool;

    DeviceRenderStageVK * m_ShadowStage[3];
    DeviceRenderStageVK * m_gPassStage;
    DeviceRenderStageVK * m_DeferredLightingStage;
    DeviceRenderStageVK * m_skyStage;
    DeviceRenderStageVK * m_fogStage;
    DeviceRenderStageVK * m_transparentStage;
    DeviceRenderStageVK * m_thumbNailRenderStage;

    DevicePipelineVK * m_dirLightingPassPiepeline;
    DevicePipelineVK * m_skyPassPipeLine;
    DevicePipelineVK * m_textureToScreenPipeline;
    std::unordered_set<DevicePipelineVK *> m_fuckingObjList;
    DeviceTextureVK * m_imguiTextureFont;
    Mesh * m_sphere;
    unsigned m_imageIndex;
    std::vector<unsigned> m_commandBufferIndex;
    RenderPath * m_renderPath;
};

} // namespace tzw

