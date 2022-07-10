#include "VkRenderBackEnd.h"

#include <assert.h>
#include <vector>
#define GLFW_INCLUDE_NONE

#include "GLFW/glfw3.h"
#include "Utility/file/Tfile.h"
#include "Engine/Engine.h"
#include <EngineSrc\Math\Matrix44.h>
#include <EngineSrc\Mesh\VertexData.h>
#include <array>
#include <iostream>
#include "Technique/MaterialPool.h"

#include "SOIL2/stb_image.h"
#include "vk/DeviceTextureVK.h"
#include "vk/DeviceShaderCollectionVK.h"
#include "vk/DeviceBufferVK.h"
#include "vk/DeviceRenderPassVK.h"
#include "Technique/Material.h"



#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/Include/Types.h"
#include "glslang/public/ShaderLang.h"
#include "Mesh/Mesh.h"
#include "Mesh/InstancedMesh.h"
#include "2D/GUISystem.h"
#include "Texture/TextureMgr.h"
#include "Utility/log/Log.h"

#include "3D/Primitive/SpherePrimitive.h"
#include <EngineSrc\Scene\SceneMgr.h>

#include "Base/TAssert.h"
#include "3D/Thumbnail.h"
#include "../3D/ShadowMap/ShadowMap.h"
#include "Rendering/InstancingMgr.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Scene/SceneCuller.h"
#include "Rendering/GraphicsRenderer.h"
#include "vk/DeviceMaterialVK.h"

//#include "vk/DeviceShaderCollectionVK.h"
namespace tzw
{


#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
const bool enableValidationLayers = true;
const int MAX_FRAMES_IN_FLIGHT = 1;
static void initVk()
{
    
}



static PFN_vkCmdBeginDebugUtilsLabelEXT g_CMD_BEGIN_DEBUG_UTILS_LABEL = nullptr;
static PFN_vkCmdEndDebugUtilsLabelEXT g_CMD_END_DEBUG_UTILS_LABEL = nullptr;
static PFN_vkSetDebugUtilsObjectNameEXT g_DEBUG_UTILS_NAME = nullptr;
static FrameBufferVK offScreenFrameBuf;
std::vector<const char*> getRequiredExtensions() {
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

    //always open debug region
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

PFN_vkCreateDebugReportCallbackEXT my_vkCreateDebugReportCallbackEXT = NULL; 

FILE * vulkanValidationFile = nullptr;


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation",
};

VkShaderModule VulkanCreateShaderModule(VkDevice& device, const char* pFileName)
{
    int codeSize = 0;
    auto data = Tfile::shared()->getData(pFileName, false);
    char* pShaderCode = (char *)data.getBytes();//ReadBinaryFile(pFileName, codeSize);
    codeSize = data.getSize();
    assert(pShaderCode);


    


    
    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = codeSize;
    shaderCreateInfo.pCode = (const uint32_t*)pShaderCode;
    
    VkShaderModule shaderModule;
    VkResult res = vkCreateShaderModule(device, &shaderCreateInfo, NULL, &shaderModule);
    CHECK_VULKAN_ERROR("vkCreateShaderModule error %d\n", res);
    printf("Created shader %s\n", pFileName);
    return shaderModule;    
}

bool VKRenderBackEnd::memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) 
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layerAAAA: " << pCallbackData->pMessage << std::endl;
        if(!vulkanValidationFile){
            vulkanValidationFile = fopen("./vulkanLog.txt", "w");
        }
        const char * errorPattern = "Validation Error:";
        //const char * errorPattern = "Cannot call vkUpdateDescriptorSets() to perform write update";
        fprintf(vulkanValidationFile, "validation Layer %s\n", pCallbackData->pMessage);
	    if(strncmp(pCallbackData->pMessage, errorPattern, strlen(errorPattern)) == 0)
	    {
            std::string tmpStr = pCallbackData->pMessage;
            if(tmpStr.find("Cannot call vkUpdateDescriptorSets() to perform write update") != std::string::npos)
            {
                printf("check\n");
            }
		    printf("hehe\n");
		    //abort();
	    }
        return VK_FALSE;
    }
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback2(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}
static VkSurfaceKHR createVKSurface(VkInstance* instance, GLFWwindow * window)
{
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(*instance, window, NULL, &surface);
    if (err)
    {
        printf("aaaaa%d\n",err);
        abort();
        // Window surface creation failed
    }
    return surface;
}
	VKRenderBackEnd::VKRenderBackEnd()
	{
	    m_imguiPipeline = nullptr;
        m_renderPath = new RenderPath();
	}
    void VKRenderBackEnd::updateItemDescriptor(VkDescriptorSet itemDescSet, Material* mat, size_t m_offset, size_t bufferRange)
    {
        auto shader = static_cast<DeviceShaderCollectionVK * >(mat->getProgram()->getDeviceShader());
        auto theList = shader->getSetInfo()[1];
        auto theSize = theList.size();
        int count = 0;
        auto & varList = mat->getVarList();
        std::vector<VkWriteDescriptorSet> descriptorWrites{};
        //update descriptor
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = VKRenderBackEnd::shared()->getItemBufferPool()->getBuffer()->getBuffer();
        bufferInfo.offset = m_offset;
        bufferInfo.range = bufferRange;

        VkWriteDescriptorSet writeSet{};
        writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.pNext = nullptr;
        writeSet.dstSet = itemDescSet;
        writeSet.dstBinding = 0;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeSet.descriptorCount = 1;
        writeSet.pBufferInfo = &bufferInfo;
        count += 1;
		vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), 1, &writeSet, 0, nullptr);
        if(count != theSize){
            abort();
        
        }
    }
    void VKRenderBackEnd::blitTexture(VkCommandBuffer command, DeviceTextureVK* srcTex, DeviceTextureVK* dstTex, vec2 size, VkImageLayout srcLayout, VkImageLayout dstLayout)
    {

        VkPipelineStageFlags srcStageFlag = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        VkAccessFlags srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        getStageAndAcessMaskFromLayOut(srcLayout, srcStageFlag, srcAccessMask);

        VkPipelineStageFlags dstStageFlag = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        VkAccessFlags dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        getStageAndAcessMaskFromLayOut(dstLayout, dstStageFlag, dstAccessMask);

		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		TAssert(srcTex->getTextureRole() == dstTex->getTextureRole(), "both texture must have same role!");

		if(srcTex->getTextureRole() == TextureRoleEnum::AS_COLOR)
		{
			aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		}
        //我们要拷贝Gbuffer里的深度到LightPass的深度里，好jb麻烦，首先要把Gbuffer的depth layout从shader_read转成transfer_src，把lightPass的depth从depth_attachment转
        //成TransDepth，然后再blit,blit结束后，还要再把这两个图转到各自的layOut(前者是shader_read，后者仍然还是深度attachment)，是个夹心饼干的做法
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = srcTex->getImage();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlag;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.oldLayout = srcLayout;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(command,
            srcStageFlag, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = dstTex->getImage();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlag;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.oldLayout = dstLayout;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcAccessMask = dstAccessMask;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(command,
            dstStageFlag, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        //blit the Gbuffer depth to deferredlight light pass depth.
        vec2 blitSize = size;
        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {(int32_t)blitSize.x, (int32_t)blitSize.y, 1};
        blit.srcSubresource.aspectMask = aspectFlag;
        blit.srcSubresource.mipLevel = 0;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {(int32_t)blitSize.x, (int32_t)blitSize.y, 1};
        blit.dstSubresource.aspectMask = aspectFlag;
        blit.dstSubresource.mipLevel = 0;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(command, srcTex->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
            dstTex->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_NEAREST);
            
 
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = srcTex->getImage();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlag;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = srcLayout;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = srcAccessMask;
        vkCmdPipelineBarrier(command,
            VK_PIPELINE_STAGE_TRANSFER_BIT, srcStageFlag, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
            
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = dstTex->getImage();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlag;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = dstLayout;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = dstAccessMask;
        vkCmdPipelineBarrier(command,
            VK_PIPELINE_STAGE_TRANSFER_BIT, dstStageFlag, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }
    DeviceFrameBufferVK* VKRenderBackEnd::createSwapChainFrameBuffer(int index)
    {
        return m_screenFrameBuffer[index];
    }
    int VKRenderBackEnd::getCurrSwapIndex()
    {
        return m_imageIndex;
    }
    DeviceRenderPassVK* VKRenderBackEnd::getScreenRenderPass()
    {
        return m_screenRenderPass;
    }
    static int regionCount = 0;
    VkCommandBuffer lastCmd = nullptr;
    void VKRenderBackEnd::beginDebugRegion(VkCommandBuffer cmd, const char* labelStr)
    {
        if(regionCount != 0)
        {
            abort();
        }
        regionCount = 1;
        VkDebugUtilsLabelEXT theLabel = {};
        theLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        theLabel.pLabelName = labelStr;
        g_CMD_BEGIN_DEBUG_UTILS_LABEL(cmd, &theLabel);
        lastCmd = cmd;
    }

    void VKRenderBackEnd::endDebugRegion(VkCommandBuffer cmd)
    {
        if(regionCount != 1)
        {
            abort();
        }
        regionCount = 0;
        if(lastCmd != cmd)
        {
            abort();
        }
        g_CMD_END_DEBUG_UTILS_LABEL(cmd);
    }

    void VKRenderBackEnd::VulkanEnumExtProps(std::vector<VkExtensionProperties>& ExtProps)
    {
        unsigned NumExt = 0;
        VkResult res = vkEnumerateInstanceExtensionProperties(NULL, &NumExt, NULL);
        CHECK_VULKAN_ERROR("vkEnumerateInstanceExtensionProperties error %d\n", res);
    
        printf("Found %d extensions\n", NumExt);
    
        ExtProps.resize(NumExt);

        res = vkEnumerateInstanceExtensionProperties(NULL, &NumExt, &ExtProps[0]);
        CHECK_VULKAN_ERROR("vkEnumerateInstanceExtensionProperties error %d\n", res);        
    
        for (unsigned i = 0 ; i < NumExt ; i++) {
            printf("Instance extension %d - %s\n", i, ExtProps[i].extensionName);
        }
    }
    void VKRenderBackEnd::CreateInstance()
    {
VkApplicationInfo appInfo = {};       
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "TinyBuilder";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;
    auto vkExtensionList = getRequiredExtensions();
   
    const char* pInstLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    
    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
    if(m_isEnableValidation)
    {
        instInfo.enabledLayerCount = ARRAY_SIZE_IN_ELEMENTS(pInstLayers);
        instInfo.ppEnabledLayerNames = pInstLayers;
    }

  
    instInfo.enabledExtensionCount = vkExtensionList.size();//ARRAY_SIZE_IN_ELEMENTS(pInstExt);
    instInfo.ppEnabledExtensionNames = vkExtensionList.data();         

    VkResult res = vkCreateInstance(&instInfo, NULL, &m_inst);
    CHECK_VULKAN_ERROR("vkCreateInstance %d\n", res);
    if(m_isEnableValidation)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;


        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_inst, "vkCreateDebugUtilsMessengerEXT");
        if(func(m_inst, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            abort();
        }

    }

    g_CMD_BEGIN_DEBUG_UTILS_LABEL = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_inst, "vkCmdBeginDebugUtilsLabelEXT");

    g_CMD_END_DEBUG_UTILS_LABEL = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_inst, "vkCmdEndDebugUtilsLabelEXT");

    g_DEBUG_UTILS_NAME = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_inst, "vkSetDebugUtilsObjectNameEXT");
    }
    void VKRenderBackEnd::VulkanGetPhysicalDevices(const VkInstance& inst, const VkSurfaceKHR& Surface, VulkanPhysicalDevices& PhysDevices)
    {
        unsigned NumDevices = 0;
    
        VkResult res = vkEnumeratePhysicalDevices(inst, &NumDevices, NULL);
        printf("vkEnumeratePhysicalDevices error %d\n", res);
        printf("Num physical devices %d\n", NumDevices);

        PhysDevices.m_devices.resize(NumDevices);
        PhysDevices.m_devProps.resize(NumDevices);
        PhysDevices.m_qFamilyProps.resize(NumDevices);
        PhysDevices.m_qSupportsPresent.resize(NumDevices);
        PhysDevices.m_surfaceFormats.resize(NumDevices);
        PhysDevices.m_surfaceCaps.resize(NumDevices);

        res = vkEnumeratePhysicalDevices(inst, &NumDevices, &PhysDevices.m_devices[0]);
        CHECK_VULKAN_ERROR("vkEnumeratePhysicalDevices error %d\n", res);

        for (unsigned i = 0 ; i < NumDevices ; i++) 
        {
                const VkPhysicalDevice& PhysDev = PhysDevices.m_devices[i];
                vkGetPhysicalDeviceProperties(PhysDev, &PhysDevices.m_devProps[i]);
                printf("Device name: %s\n", PhysDevices.m_devProps[i].deviceName);
                uint32_t apiVer = PhysDevices.m_devProps[i].apiVersion;
                printf("    API version: %d.%d.%d\n", VK_VERSION_MAJOR(apiVer),
                                                  VK_VERSION_MINOR(apiVer),
                                                  VK_VERSION_PATCH(apiVer));
                 unsigned NumQFamily = 0;         
        
                vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &NumQFamily, NULL);
    
                printf("    Num of family queues: %d\n", NumQFamily);

                PhysDevices.m_qFamilyProps[i].resize(NumQFamily);
                PhysDevices.m_qSupportsPresent[i].resize(NumQFamily);

                vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &NumQFamily, &(PhysDevices.m_qFamilyProps[i][0]));
            for (unsigned q = 0 ; q < NumQFamily ; q++) {
                res = vkGetPhysicalDeviceSurfaceSupportKHR(PhysDev, q, Surface, &(PhysDevices.m_qSupportsPresent[i][q]));
                CHECK_VULKAN_ERROR("vkGetPhysicalDeviceSurfaceSupportKHR error %d\n", res);
            }
            unsigned NumFormats = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(PhysDev, Surface, &NumFormats, NULL);
            assert(NumFormats > 0);
        
            PhysDevices.m_surfaceFormats[i].resize(NumFormats);
        
            res = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysDev, Surface, &NumFormats, &(PhysDevices.m_surfaceFormats[i][0]));
            CHECK_VULKAN_ERROR("vkGetPhysicalDeviceSurfaceFormatsKHR error %d\n", res);
    
            for (unsigned j = 0 ; j < NumFormats ; j++) {
                const VkSurfaceFormatKHR& SurfaceFormat = PhysDevices.m_surfaceFormats[i][j];
                printf("    Format %d color space %d\n", SurfaceFormat.format , SurfaceFormat.colorSpace);
            }
            res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysDev, Surface, &(PhysDevices.m_surfaceCaps[i]));
            CHECK_VULKAN_ERROR("vkGetPhysicalDeviceSurfaceCapabilitiesKHR error %d\n", res);
       
            //VulkanPrintImageUsageFlags(PhysDevices.m_surfaceCaps[i].supportedUsageFlags);
        }
    }


    VkInstance& VKRenderBackEnd::getVKInstance()
    {
        return m_inst;
    }

    void VKRenderBackEnd::setVKSurface(const VkSurfaceKHR& surface)
    {
        m_surface = surface;
    }

    VkSurfaceKHR& VKRenderBackEnd::getVKSurface()
    {
        return m_surface;
    }

    void VKRenderBackEnd::SelectPhysicalDevice()
    {
        for (unsigned i = 0 ; i < m_physicsDevices.m_devices.size() ; i++) {
                
            for (unsigned j = 0 ; j < m_physicsDevices.m_qFamilyProps[i].size() ; j++) {
                VkQueueFamilyProperties& QFamilyProp = m_physicsDevices.m_qFamilyProps[i][j];
            
                printf("Family %d Num queues: %d\n", j, QFamilyProp.queueCount);
                VkQueueFlags flags = QFamilyProp.queueFlags;
                printf("    GFX %s, Compute %s, Transfer %s, Sparse binding %s\n",
                        (flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No",
                        (flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No",
                        (flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No",
                        (flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No");
            
                if (flags & VK_QUEUE_GRAPHICS_BIT) {
                    if (!m_physicsDevices.m_qSupportsPresent[i][j]) {
                        printf("Present is not supported\n");
                        continue;
                    }

                    m_gfxDevIndex = i;
                    m_gfxQueueFamily = j;
                    printf("Using GFX device %d and queue family %d\n", m_gfxDevIndex, m_gfxQueueFamily);
                    break;
                }
            }
        }
    
        if (m_gfxDevIndex == -1) {
            printf("No GFX device found!\n");
            assert(0);
        }    
    }

    void VKRenderBackEnd::CreateLogicalDevice()
    {
        float qPriorities = 1.0f;
        VkDeviceQueueCreateInfo qInfo = {};
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = m_gfxQueueFamily;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &qPriorities;
    
        const char* pDevExt[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    
        VkDeviceCreateInfo devInfo = {};
        devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devInfo.enabledExtensionCount = ARRAY_SIZE_IN_ELEMENTS(pDevExt);
        devInfo.ppEnabledExtensionNames = pDevExt;
        devInfo.queueCreateInfoCount = 1;
        devInfo.pQueueCreateInfos = &qInfo;
        VkResult res = vkCreateDevice(GetPhysDevice(), &devInfo, NULL, &m_device);

        CHECK_VULKAN_ERROR("vkCreateDevice error %d\n", res);
   
        printf("Device created\n");
    }
    void VKRenderBackEnd::createSwapChain()
    {
        const VkSurfaceCapabilitiesKHR& SurfaceCaps = GetSurfaceCaps();
        assert(SurfaceCaps.currentExtent.width != -1);
                   
        unsigned NumImages = 2;

        assert(NumImages >= SurfaceCaps.minImageCount);
        assert(NumImages <= SurfaceCaps.maxImageCount);
      
        VkSwapchainCreateInfoKHR SwapChainCreateInfo = {};
    
        SwapChainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        SwapChainCreateInfo.surface          = m_surface;
        SwapChainCreateInfo.minImageCount    = NumImages;
        SwapChainCreateInfo.imageFormat      = GetSurfaceFormat().format;
        SwapChainCreateInfo.imageColorSpace  = GetSurfaceFormat().colorSpace;
        SwapChainCreateInfo.imageExtent      = SurfaceCaps.currentExtent;
        SwapChainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        SwapChainCreateInfo.preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        SwapChainCreateInfo.imageArrayLayers = 1;
        SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        SwapChainCreateInfo.presentMode      = VK_PRESENT_MODE_IMMEDIATE_KHR;
        SwapChainCreateInfo.clipped          = true;
        SwapChainCreateInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    
        VkResult res = vkCreateSwapchainKHR(m_device, &SwapChainCreateInfo, NULL, &m_swapChainKHR);
        CHECK_VULKAN_ERROR("vkCreateSwapchainKHR error %d\n", res);    

        printf("Swap chain created\n");
    
        unsigned NumSwapChainImages = 0;
        res = vkGetSwapchainImagesKHR(m_device, m_swapChainKHR, &NumSwapChainImages, NULL);
        CHECK_VULKAN_ERROR("vkGetSwapchainImagesKHR error %d\n", res);
        assert(NumImages == NumSwapChainImages);
    
        printf("Number of images %d\n", NumSwapChainImages);

        m_images.resize(NumSwapChainImages);
        m_views.resize(NumSwapChainImages);
        m_cmdBufs.resize(NumSwapChainImages);
    
        res = vkGetSwapchainImagesKHR(m_device, m_swapChainKHR, &NumSwapChainImages, &(m_images[0]));
        CHECK_VULKAN_ERROR("vkGetSwapchainImagesKHR error %d\n", res);



        vkGetPhysicalDeviceMemoryProperties(GetPhysDevice(), &memory_properties);
    }
    void VKRenderBackEnd::CreateCommandBuffer()
    {
            VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
            cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            cmdPoolCreateInfo.queueFamilyIndex = m_gfxQueueFamily;
    
            VkResult res = vkCreateCommandPool(m_device, &cmdPoolCreateInfo, NULL, &m_cmdBufPool);    
            CHECK_VULKAN_ERROR("vkCreateCommandPool error %d\n", res);
    
            printf("Command buffer pool created\n");
    
            VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
            cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufAllocInfo.commandPool = m_cmdBufPool;
            cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufAllocInfo.commandBufferCount = m_images.size();
    
            res = vkAllocateCommandBuffers(m_device, &cmdBufAllocInfo, &m_cmdBufs[0]);            
            CHECK_VULKAN_ERROR("vkAllocateCommandBuffers error %d\n", res);
    
            printf("Created command buffers\n");
    }

    void VKRenderBackEnd::RecordCommandBuffers()
        {  
    }

    void VKRenderBackEnd::CreateRenderPass()
    {
        m_screenRenderPass = new DeviceRenderPassVK();
        m_screenRenderPass->init({{ImageFormat::Surface_Format, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPassVK::OpType::LOADCLEAR_AND_STORE, false, true);
    }

    void VKRenderBackEnd::CreateTextureToScreenRenderPass()
    {
    }
    
    void VKRenderBackEnd::createDeferredFrameBuffer()
    {

    }
    void VKRenderBackEnd::createAttachment(VkFormat format, VkImageUsageFlagBits usage, FrameBufferAttachmentVK* attachment)
    {
    }
    void VKRenderBackEnd::CreateFramebuffer()
    {
        printf("try create Frame Buffer\n");
        VkResult res;
        m_screenTexs.resize(m_images.size());
        for (unsigned i = 0 ; i < m_images.size() ; i++) {
            VkImageViewCreateInfo ViewCreateInfo = {};
            ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ViewCreateInfo.image = m_images[i];
            ViewCreateInfo.format = GetSurfaceFormat().format;
            ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            ViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            ViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            ViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ViewCreateInfo.subresourceRange.baseMipLevel = 0;
            ViewCreateInfo.subresourceRange.levelCount = 1;
            ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            ViewCreateInfo.subresourceRange.layerCount = 1;    

            res = vkCreateImageView(m_device, &ViewCreateInfo, NULL, &m_views[i]);
            CHECK_VULKAN_ERROR("vkCreateImageView error %d\n", res);
            m_screenTexs[i] = new DeviceTextureVK(m_images[i], m_views[i]);
        }
        m_screenDepth = new DeviceTextureVK(depthImage, depthImageView);
        m_screenFrameBuffer.resize(m_images.size());
        for(unsigned i = 0; i < m_images.size(); i++)
        {
            m_screenFrameBuffer[i] = new DeviceFrameBufferVK();
            m_screenFrameBuffer[i]->init(m_screenTexs[i], m_screenDepth, m_screenRenderPass);
        }
        printf("Frame buffers created\n");
    }
    void VKRenderBackEnd::CreateShaders()
    {
        m_vsModule = VulkanCreateShaderModule(m_device, "VulkanShaders/vs.spv");
        assert(m_vsModule);

        m_fsModule = VulkanCreateShaderModule(m_device, "VulkanShaders/fs.spv");
        assert(m_fsModule);

        m_shader = new DeviceShaderCollectionVK();
        m_shader->create();
        tzw::Data data = tzw::Tfile::shared()->getData("VulkanShaders/vulkanTest_v.glsl",false);
        m_shader->addShader((const unsigned char *)data.getBytes(),data.getSize(),DeviceShaderType::VertexShader,(const unsigned char *)"vulkanTest_v.glsl");

        tzw::Data data2 = tzw::Tfile::shared()->getData("VulkanShaders/vulkanTest_f.glsl",false);
        m_shader->addShader((const unsigned char *)data2.getBytes(),data2.getSize(),DeviceShaderType::FragmentShader,(const unsigned char *)"vulkanTest_f.glsl");

        //shader 创建完毕，根据shader内容构建描述符集布局
        m_shader->finish();
    }
    void VKRenderBackEnd::CreatePipeline()
    {
        auto computeShader = new DeviceShaderCollectionVK();
        tzw::Data data = tzw::Tfile::shared()->getData("VulkanShaders/VulkanTestCompute.glsl",false);
        computeShader->addShader((const unsigned char *)data.getBytes(),data.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"VulkanTestCompute.glsl");
        computeShader->finish();
        auto computePipeline = createPipeline_imp();
        computePipeline->initCompute(computeShader);
    }

    void VKRenderBackEnd::CreateUiniform()
    {
        VkDeviceSize bufferSize = sizeof(Matrix44);

        uniformBuffers.resize(m_images.size());
        uniformBuffersMemory.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); i++) 
        {
            createVKBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }
    void VKRenderBackEnd::CreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions)
    {

        attributeDescriptions.resize(3);

		//local position
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexData, m_pos);


		//color
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexData, m_color);
	
		//uv
	    attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexData, m_texCoord);

    }
    void VKRenderBackEnd::CreateVertexBuffer()
    {
	vec4 m_color = vec4(1, 1, 1, 1);
	vec4 m_color1 = vec4(0, 1, 0, 1);
	vec4 m_color2 = vec4(0, 0, 1, 1);
	vec4 m_color3 = vec4(1, 0, 0, 1);
	vec4 m_color4 = vec4(1, 0, 1, 1);
	vec4 m_color5 = vec4(1, 1, 0, 1);

	float halfWidth = 0.5;
	float halfDepth = 0.5;
	float halfHeight = 0.5;

	
    VertexData vertices[] = {
        // Vertex data for face 0
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.0f), m_color),  // v0
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(1.f, 0.0f), m_color), // v1
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 1.f), m_color),  // v2
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(1.f, 1.f), m_color), // v3

        // Vertex data for face 1
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2( 0.0f, 0.0f), m_color1), // v4
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(1.f, 0.0f), m_color1), // v5
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 1.0f), m_color1),  // v6
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(1.f, 1.0f), m_color1), // v7

        // Vertex data for face 2
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.0f, 0.0f), m_color2), // v8
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 0.0f), m_color2),  // v9
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.0f, 1.0f), m_color2), // v10
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 1.0f), m_color2),  // v11

        // Vertex data for face 3
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.0f, 0.0f), m_color3), // v12
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 0.0f), m_color3),  // v13
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.0f, 1.f), m_color3), // v14
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 1.f), m_color3),  // v15

        // Vertex data for face 4
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.f, 0.0f), m_color4), // v16
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(1.f, 0.0f), m_color4), // v17
        VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 1.0f), m_color4), // v18
        VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 1.0f), m_color4), // v19

        // Vertex data for face 5
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.0f), m_color5), // v20
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 0.0f), m_color5), // v21
        VertexData(vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.0f, 1.0f), m_color5), // v22
        VertexData(vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 1.0f), m_color5)  // v23
    };
	
        auto buffer = createBuffer_imp();
        buffer->init(DeviceBufferType::Vertex);

        buffer->allocate(vertices, sizeof(vertices[0]) * ARRAY_SIZE_IN_ELEMENTS(vertices));
        m_vertexBuffer = static_cast<DeviceBufferVK *>(buffer);
    }

    void VKRenderBackEnd::createDepthResources()
	{
        VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;//findDepthFormat();
		auto screenSize = Engine::shared()->winSize();
        createImage(screenSize.x, screenSize.y, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void VKRenderBackEnd::createCommandBufferForGeneral(int size)
    {
        m_generalCmdBuff.resize(m_images.size());
        m_commandBufferIndex.resize(m_images.size());

        for(int i = 0;i <m_images.size(); i++)
        {
            m_commandBufferIndex[i] = 0;
        	std::vector<VkCommandBuffer> tmpVec;
            tmpVec.resize(size);
            VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
            cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufAllocInfo.commandPool = m_generalCmdBufPool;
            cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufAllocInfo.commandBufferCount = size;
    
            auto res = vkAllocateCommandBuffers(m_device, &cmdBufAllocInfo, tmpVec.data());
            if(res){
                abort();
            }

        	for(auto cmd : tmpVec)
        	{
        		DeviceRenderCommandVK vk(cmd);
        		m_generalCmdBuff[i].push_back(vk);
        	}
        }

    }

    void VKRenderBackEnd::createCommandBufferPoolForGeneral()
    {
        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.flags= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolCreateInfo.queueFamilyIndex = m_gfxQueueFamily;
        VkResult res = vkCreateCommandPool(m_device, &cmdPoolCreateInfo, NULL, &m_generalCmdBufPool);    
        CHECK_VULKAN_ERROR("vkCreateCommandPool error %d\n", res);
    }



    void VKRenderBackEnd::CreateIndexBuffer()
    {

        indices = {
         0,  1,  2,  1,  3,  2,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  5,  6,  5,  7,  6, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  9,  10, 9, 11, 10, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 13, 14, 13, 15, 14, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 17, 18, 17, 19, 18, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 21, 22, 21, 23, 22,      // Face 5 - triangle strip (v20, v21, v22, v23)
		};

	/*
		indices = {
		    0, 1, 2, 2, 3, 0,
		    4, 5, 6, 6, 7, 4
		};
	*/

        auto buffer = createBuffer_imp();
        buffer->init(DeviceBufferType::Index);

        buffer->allocate(indices.data(), sizeof(indices[0]) * indices.size());
        m_indexBuffer = static_cast<DeviceBufferVK *>(buffer);
    }

    void VKRenderBackEnd::CreateDescriptorSetLayout()
    {
        descriptorSetLayout = m_shader->getMaterialDescriptorSetLayOut();
        return;
    }
    void VKRenderBackEnd::createDescriptorPool()
    {
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 256;

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 256;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 512;
        if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
    void VKRenderBackEnd::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(m_images.size(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_images.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(m_images.size());
        if (vkAllocateDescriptorSets(m_device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            abort();
        }
        printf("createDescriptorSets 111 %p %p\n",descriptorSets[0], descriptorSets[1]);

        for (size_t i = 0; i < m_images.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(Matrix44);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_myTexture->getImageView();
            imageInfo.sampler = textureSampler;

        	
            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;
            vkUpdateDescriptorSets(m_device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
    void VKRenderBackEnd::createVKBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto res = vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer);
        assert(!res);
        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.memoryTypeIndex = 0;
        alloc_info.allocationSize = memRequirements.size;
        bool pass = memory_type_from_properties(memRequirements.memoryTypeBits,
                                           properties,
                                           &alloc_info.memoryTypeIndex);
        res = vkAllocateMemory(m_device, &alloc_info, NULL,
                               &(bufferMemory));
        assert(!res);
        res = vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
        assert(!res);

    }

    DeviceMemoryPoolVK* VKRenderBackEnd::getMemoryPool()
    {
        return m_memoryPool;
    }

    VkFormat VKRenderBackEnd::getFormat(ImageFormat imageFormat)
    {
        switch(imageFormat){
        case ImageFormat::R8:
            return VK_FORMAT_R8_UNORM;
        case ImageFormat::R8G8:
            return VK_FORMAT_R8G8_UNORM;
        case ImageFormat::R8G8B8:
            return VK_FORMAT_R8G8B8_UNORM;
        case ImageFormat::R8G8B8A8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::R8G8B8A8_S:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case ImageFormat::R16G16B16A16:
            return VK_FORMAT_R16G16B16A16_UNORM;
        case ImageFormat::D16:
            return VK_FORMAT_D16_UNORM;
        case ImageFormat::D24_S8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case ImageFormat::D16_S8:
            return VK_FORMAT_D16_UNORM_S8_UINT;
        case ImageFormat::R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::Surface_Format:
            return GetSurfaceFormat().format;
        }
        return VK_FORMAT_R8G8B8A8_UNORM;
    }

void VKRenderBackEnd::createTextureImage()
{
    m_myTexture = new DeviceTextureVK("logo.png");
}

VkCommandBuffer VKRenderBackEnd::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_cmdBufPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VKRenderBackEnd::createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(m_images.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
 }

void VKRenderBackEnd::initVMAPool()
{
    m_memoryPool = new DeviceMemoryPoolVK(GetPhysDevice(), m_device, m_inst);
}
void VKRenderBackEnd::shadowPass()
{
    ShadowMap::shared()->calculateProjectionMatrix();

    for (int i = 0 ; i < SHADOWMAP_CASCADE_NUM ; i++)
    {
        auto aabb = ShadowMap::shared()->getPotentialRange(i);
    
    
    }
}

void VKRenderBackEnd::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_queue);
    vkFreeCommandBuffers(m_device, m_cmdBufPool, 1, &commandBuffer);
}

void VKRenderBackEnd::prepareFrame()
{
    if (imagesInFlight[m_imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(m_device, 1, &imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
    }
    VkResult res = vkAcquireNextImageKHR(m_device, m_swapChainKHR, UINT64_MAX, imageAvailableSemaphores[currentFrame], NULL, &m_imageIndex);
    clearCommandBuffer();
    m_itemBufferPool->reset();
}

void VKRenderBackEnd::endFrame(RenderPath * renderPath)
{
    std::vector<VkCommandBuffer> commandList = {};
    /*for(auto stage : renderPath->getStages())
    {
        commandList.emplace_back( static_cast<DeviceRenderStageVK *>(stage)->getCommand());
    }*/
    commandList.push_back( static_cast<DeviceRenderCommandVK *>( renderPath->getCmd())->getVK());

    if (imagesInFlight[m_imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(m_device, 1, &imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[m_imageIndex] = inFlightFences[currentFrame];


    VkSubmitInfo submitInfo = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount   = commandList.size();
    submitInfo.pCommandBuffers      = commandList.data();


    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;


    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(m_device, 1, &inFlightFences[currentFrame]);
    
    VkResult res = vkQueueSubmit(m_queue, 1, &submitInfo, inFlightFences[currentFrame]);    
    if(res){exit(0);}
    CHECK_VULKAN_ERROR("vkQueueSubmit error %d\n", res);
    
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &m_swapChainKHR;
    presentInfo.pImageIndices      = &m_imageIndex;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    res = vkQueuePresentKHR(m_queue, &presentInfo);    
    CHECK_VULKAN_ERROR("vkQueuePresentKHR error %d\n" , res);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

DeviceRenderCommand * VKRenderBackEnd::getGeneralCommandBuffer()
{
    auto cmd = &m_generalCmdBuff[m_imageIndex][m_commandBufferIndex[m_imageIndex]];
    m_commandBufferIndex[m_imageIndex] += 1;
    return cmd;
}

void VKRenderBackEnd::beginGeneralCommandBuffer()
{
	/*VkCommandBufferBeginInfo beginInfoDeffered = {};
    beginInfoDeffered.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfoDeffered.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    
    int res = vkBeginCommandBuffer(getGeneralCommandBuffer()->get(), &beginInfoDeffered);
	CHECK_VULKAN_ERROR("vkBeginCommandBuffer error %d\n", res);*/
}

void VKRenderBackEnd::endGeneralCommandBuffer()
{
    //vkEndCommandBuffer(getGeneralCommandBuffer());
}

void VKRenderBackEnd::clearCommandBuffer()
{
    for(unsigned& commandIndex : m_commandBufferIndex)
    {
        commandIndex = 0;
    }
}

std::unordered_map<Material*, DevicePipelineVK*>& VKRenderBackEnd::getPipelinePool()
{
    return m_matPipelinePool;
}

void VKRenderBackEnd::getStageAndAcessMaskFromLayOut(VkImageLayout layout, VkPipelineStageFlags& stage, VkAccessFlags& access)
{
    switch(layout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        access = 0;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        access = VK_ACCESS_SHADER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        access = VK_ACCESS_SHADER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        access = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_GENERAL:
        stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        access = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT ;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    default:
        {
            throw std::invalid_argument("unsupported layout transition!");
        }
        break;
    }
}

void VKRenderBackEnd::transitionImageLayoutUseBarrier(VkCommandBuffer cmd, DeviceTextureVK * texture,
    VkImageLayout oldLayout, VkImageLayout newLayout, int mipBase, int mipCount)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture->getImage();

    VkImageAspectFlags aspectFlags = texture->getImageAspectFlag();
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = mipBase;
    barrier.subresourceRange.levelCount = mipCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = {};
    VkPipelineStageFlags destinationStage = {};


    getStageAndAcessMaskFromLayOut(oldLayout, sourceStage, barrier.srcAccessMask);
    getStageAndAcessMaskFromLayOut(newLayout, destinationStage, barrier.dstAccessMask);
 

    vkCmdPipelineBarrier(
        cmd,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

void VKRenderBackEnd::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int baseMipLevel, int levelCount)
{
   VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    if(aspectFlags == 0)
    {
        abort();
    }
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = baseMipLevel;
    barrier.subresourceRange.levelCount = levelCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    getStageAndAcessMaskFromLayOut(oldLayout, sourceStage, barrier.srcAccessMask);
    getStageAndAcessMaskFromLayOut(newLayout, destinationStage, barrier.dstAccessMask);

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}


	void VKRenderBackEnd::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkImageAspectFlags aspectFlags, int mipLevel) 
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = aspectFlags;
        region.imageSubresource.mipLevel = mipLevel;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }
void VKRenderBackEnd::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}
const VkSurfaceFormatKHR& VKRenderBackEnd::GetSurfaceFormat() const
    {
        assert(m_gfxDevIndex >= 0);
        return m_physicsDevices.m_surfaceFormats[m_gfxDevIndex][0];
    }
    const VkSurfaceCapabilitiesKHR VKRenderBackEnd::GetSurfaceCaps() const
    {
        assert(m_gfxDevIndex >= 0);
        return m_physicsDevices.m_surfaceCaps[m_gfxDevIndex];
    }

    const VkPhysicalDevice& VKRenderBackEnd::GetPhysDevice()
    {
        assert(m_gfxDevIndex >= 0);
        return m_physicsDevices.m_devices[m_gfxDevIndex];
    }

    const VulkanPhysicalDevices VKRenderBackEnd::GetPhysDeviceWrapper()
    {
        assert(m_gfxDevIndex >= 0);
        return m_physicsDevices;
    }

    VkPhysicalDeviceProperties VKRenderBackEnd::GetPhysDeviceProperties()
    {
        assert(m_gfxDevIndex >= 0);
        return m_physicsDevices.m_devProps[m_gfxDevIndex];
    }

void VKRenderBackEnd::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, int mipLevels)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    auto result = vkCreateImage(m_device, &imageInfo, nullptr, &image);
    if (result!= VK_SUCCESS) {
        char tmp[512];
        sprintf(tmp, "create Image error %d\n",result);
        throw std::runtime_error(tmp);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

	memory_type_from_properties(memRequirements.memoryTypeBits,
                                           properties,
                                           &allocInfo.memoryTypeIndex);
    //allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_device, image, imageMemory, 0);
    VkDebugUtilsObjectNameInfoEXT info{};
    info.objectType = VK_OBJECT_TYPE_IMAGE;
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectHandle = (uint64_t)image;
    static int imageViewCount = 0;
    std::string imageViewName = "imageView";
    imageViewName += std::to_string(imageViewCount);
    imageViewCount++;
    info.pObjectName = imageViewName.c_str();
    g_DEBUG_UTILS_NAME(m_device, &info);
}

void VKRenderBackEnd::createTextureImageView()
{
	//textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB);
}

void VKRenderBackEnd::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(m_device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkImageView VKRenderBackEnd::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,int baseMipLevel, int levelCount)
	{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    if(aspectFlags==0)
    {
        abort();
    }
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
    viewInfo.subresourceRange.levelCount = levelCount;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}
void VKRenderBackEnd::initDevice(GLFWwindow * window)
	{
		std::vector<VkExtensionProperties> ExtProps;
		VulkanEnumExtProps(ExtProps);
        CreateInstance();
        //create surface from GLFW
        setVKSurface(createVKSurface(&VKRenderBackEnd::shared()->getVKInstance(), window));
        initDevices();
	}

    void VKRenderBackEnd::initDevices()
    {
        VulkanGetPhysicalDevices(m_inst, m_surface, m_physicsDevices);
        SelectPhysicalDevice();
        CreateLogicalDevice();
        vkGetDeviceQueue(m_device, m_gfxQueueFamily, 0, &m_queue);
        createSwapChain();
        CreateCommandBuffer();
        createCommandBufferPoolForGeneral();
        createCommandBufferForGeneral(100);
        CreateRenderPass();
        CreateTextureToScreenRenderPass();
		createDepthResources();
        CreateFramebuffer();
        CreateShaders();
        CreateUiniform();
        CreateDescriptorSetLayout();
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreatePipeline();
        createDescriptorPool();
        createDescriptorSets();
        createSyncObjects();
        initVMAPool();
        
        m_itemBufferPool = new DeviceItemBufferPoolVK(1024 * 1024 * 12);
        
    }

    DeviceTexture* VKRenderBackEnd::loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag)
    {
        DeviceTextureVK * texture = new DeviceTextureVK(buf, buffSize);
        return texture;
    }

    DeviceTexture* VKRenderBackEnd::loadTextureRaw_imp(const unsigned char* buf, int width, int height, ImageFormat format, unsigned int loadingFlag)
    {
        DeviceTextureVK * texture = new DeviceTextureVK();
        texture->initDataRaw(buf, width, height,format);
        return texture;
    }

    DeviceShaderCollection* VKRenderBackEnd::createShader_imp()
    {
        return new DeviceShaderCollectionVK();
    }

    DeviceBuffer* VKRenderBackEnd::createBuffer_imp()
    {
        return new DeviceBufferVK();
    }

    DevicePipeline* VKRenderBackEnd::createPipeline_imp()
    {
        return new DevicePipelineVK();
    }

    DeviceMaterial* VKRenderBackEnd::createDeviceMaterial_imp()
    {
        return new DeviceMaterialVK();
    }

    DeviceRenderPass* VKRenderBackEnd::createDeviceRenderpass_imp()
    {
        return new DeviceRenderPassVK();
    }

    DeviceRenderStage* VKRenderBackEnd::createRenderStage_imp()
    {
        return new DeviceRenderStageVK();
    }

    DeviceFrameBuffer* VKRenderBackEnd::createFrameBuffer_imp()
    {
        return new DeviceFrameBufferVK();
    }

    VkDevice VKRenderBackEnd::getDevice()
    {
        return m_device;
    }

    VkDescriptorPool& VKRenderBackEnd::getDescriptorPool()
    {
        return descriptorPool;
    }

    DeviceItemBufferPoolVK* VKRenderBackEnd::getItemBufferPool()
    {
        return m_itemBufferPool;
    }

}
