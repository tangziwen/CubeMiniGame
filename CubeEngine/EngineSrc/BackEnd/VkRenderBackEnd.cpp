#pragma once
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
namespace tzw
{

#define CHECK_VULKAN_ERROR(a, b) {if(b){printf(a, b);abort();}}
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
static void initVk()
{
    
}
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

    const char* pInstExt[] = {
#ifdef ENABLE_DEBUG_LAYERS
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif        
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32    
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else    
        VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif            
    };
    
#ifdef ENABLE_DEBUG_LAYERS    
    const char* pInstLayers[] = {
        "VK_LAYER_LUNARG_standard_validation"
    };
#endif    
    
    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;
#ifdef ENABLE_DEBUG_LAYERS    
    instInfo.enabledLayerCount = ARRAY_SIZE_IN_ELEMENTS(pInstLayers);
    instInfo.ppEnabledLayerNames = pInstLayers;
#endif    
    instInfo.enabledExtensionCount = ARRAY_SIZE_IN_ELEMENTS(pInstExt);
    instInfo.ppEnabledExtensionNames = pInstExt;         

    VkResult res = vkCreateInstance(&instInfo, NULL, &m_inst);
    CHECK_VULKAN_ERROR("vkCreateInstance %d\n", res);
        
#ifdef ENABLE_DEBUG_LAYERS
    // Get the address to the vkCreateDebugReportCallbackEXT function
    my_vkCreateDebugReportCallbackEXT = reinterpret_cast(vkGetInstanceProcAddr(m_inst, "vkCreateDebugReportCallbackEXT"));
    
    // Register the debug callback
    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
    callbackCreateInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    callbackCreateInfo.pNext       = NULL;
    callbackCreateInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                     VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                     VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
    callbackCreateInfo.pUserData   = NULL;

    VkDebugReportCallbackEXT callback;
    res = my_vkCreateDebugReportCallbackEXT(m_inst, &callbackCreateInfo, NULL, &callback);
    CheckVulkanError("my_vkCreateDebugReportCallbackEXT error %d\n", res);
#endif  
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
        // TODO: 在此处插入 return 语句
    }

    void VKRenderBackEnd::setVKSurface(const VkSurfaceKHR& surface)
    {
        m_surface = surface;
    }

    VkSurfaceKHR& VKRenderBackEnd::getVKSurface()
    {
        // TODO: 在此处插入 return 语句
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
        SwapChainCreateInfo.presentMode      = VK_PRESENT_MODE_FIFO_KHR;
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
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    
        VkClearColorValue clearColor = { 164.0f/256.0f, 30.0f/256.0f, 34.0f/256.0f, 0.0f };
        VkClearValue clearValue = {};
        clearValue.color = clearColor;
    
        VkImageSubresourceRange imageRange = {};
        imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageRange.levelCount = 1;
        imageRange.layerCount = 1;
        auto screenSize = Engine::shared()->winSize();
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;   
        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;
        renderPassInfo.renderArea.extent.width = screenSize.x;
        renderPassInfo.renderArea.extent.height = screenSize.y;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        for (unsigned i = 0 ; i < m_cmdBufs.size() ; i++) {            
            VkResult res = vkBeginCommandBuffer(m_cmdBufs[i], &beginInfo);
            CHECK_VULKAN_ERROR("vkBeginCommandBuffer error %d\n", res);
            renderPassInfo.framebuffer = m_fbs[i];

            vkCmdBeginRenderPass(m_cmdBufs[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(m_cmdBufs[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(m_cmdBufs[i], 0, 1, vertexBuffers, offsets);
            
            //vkCmdBindIndexBuffer(m_cmdBufs[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
            vkCmdDraw(m_cmdBufs[i], 3, 1, 0, 0);
            //vkCmdDrawIndexed(m_cmdBufs[i], static_cast<uint32_t>(6), 1, 0, 0, 0);
            vkCmdEndRenderPass(m_cmdBufs[i]);
               
            res = vkEndCommandBuffer(m_cmdBufs[i]);
            CHECK_VULKAN_ERROR("vkEndCommandBuffer error %d\n", res);
        }
    
        printf("Command buffers recorded\n");    
    }
    void VKRenderBackEnd::CreateRenderPass()
    {
        VkAttachmentReference attachRef = {};
        attachRef.attachment = 0;
        attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDesc = {};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &attachRef;

        VkAttachmentDescription attachDesc = {};    
        attachDesc.format = GetSurfaceFormat().format;
        attachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachDesc.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachDesc.samples = VK_SAMPLE_COUNT_1_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &attachDesc;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDesc;
        
        VkResult res = vkCreateRenderPass(m_device, &renderPassCreateInfo, NULL, &m_renderPass);
        CHECK_VULKAN_ERROR("vkCreateRenderPass error %d\n", res);

        printf("Created a render pass\n");
    }
    void VKRenderBackEnd::CreateFramebuffer()
    {
        printf("try create Frame Buffer\n");
        m_fbs.resize(m_images.size());
    
        VkResult res;
            
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
        
            auto screenSize = Engine::shared()->winSize();
            VkFramebufferCreateInfo fbCreateInfo = {};
            fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbCreateInfo.renderPass = m_renderPass;
            fbCreateInfo.attachmentCount = 1;
            fbCreateInfo.pAttachments = &m_views[i];
            fbCreateInfo.width = screenSize.x;
            fbCreateInfo.height = screenSize.y;
            fbCreateInfo.layers = 1;

            res = vkCreateFramebuffer(m_device, &fbCreateInfo, NULL, &m_fbs[i]);
            CHECK_VULKAN_ERROR("vkCreateFramebuffer error %d\n", res);
        }
   
        printf("Frame buffers created\n");
    }
    void VKRenderBackEnd::CreateShaders()
    {
        m_vsModule = VulkanCreateShaderModule(m_device, "VulkanShaders/vs.spv");
        assert(m_vsModule);

        m_fsModule = VulkanCreateShaderModule(m_device, "VulkanShaders/fs.spv");
        assert(m_fsModule);
    }
    void VKRenderBackEnd::CreatePipeline()
    {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {};
    
        shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStageCreateInfo[0].module = m_vsModule;
        shaderStageCreateInfo[0].pName = "main";
        shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageCreateInfo[1].module = m_fsModule;
        shaderStageCreateInfo[1].pName = "main";   
	    
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        //create vertex input
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> attributeDecsriptionList;
        CreateVertexBufferDescription(attributeDecsriptionList);

        CreateVertexBuffer();
        CreateIndexBuffer();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDecsriptionList.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDecsriptionList.data();

        VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
        pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        auto screenSize = Engine::shared()->winSize();
        VkViewport vp = {};
        vp.x = 0.0f;
        vp.y = 0.0f;
        vp.width  = (float)screenSize.x;
        vp.height = (float)screenSize.y;
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;
        
        VkPipelineViewportStateCreateInfo vpCreateInfo = {};
        vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vpCreateInfo.viewportCount = 1;
        vpCreateInfo.pViewports = &vp;
       
        VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
        rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rastCreateInfo.lineWidth = 1.0f;
    
        VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
        pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
        VkPipelineColorBlendAttachmentState blendAttachState = {};
        blendAttachState.colorWriteMask = 0xf;
    
        VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
        blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        blendCreateInfo.attachmentCount = 1;
        blendCreateInfo.pAttachments = &blendAttachState;
 
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = ARRAY_SIZE_IN_ELEMENTS(shaderStageCreateInfo);
        pipelineInfo.pStages = &shaderStageCreateInfo[0];
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pipelineIACreateInfo;
        pipelineInfo.pViewportState = &vpCreateInfo;
        pipelineInfo.pRasterizationState = &rastCreateInfo;
        pipelineInfo.pMultisampleState = &pipelineMSCreateInfo;
        pipelineInfo.pColorBlendState = &blendCreateInfo;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.basePipelineIndex = -1;
    
        VkResult res = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &m_pipeline);
        CHECK_VULKAN_ERROR("vkCreateGraphicsPipelines error %d\n", res);
    
        printf("Graphics pipeline created\n");
    }
    void VKRenderBackEnd::CreateUiniform()
    {

        Matrix44 mat;
        //create Buffer
        VkBufferCreateInfo buf_info = {};
        buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buf_info.pNext = NULL;
        buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buf_info.size = sizeof(Matrix44);
        buf_info.queueFamilyIndexCount = 0;
        buf_info.pQueueFamilyIndices = NULL;
        buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buf_info.flags = 0;
        VkBuffer buf;
        auto res = vkCreateBuffer(m_device, &buf_info, NULL, &buf);
        assert(res == VK_SUCCESS);


        //createMemory
        VkMemoryRequirements mem_reqs;
        vkGetBufferMemoryRequirements(m_device, buf,
                                      &mem_reqs);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.memoryTypeIndex = 0;

        alloc_info.allocationSize = mem_reqs.size;
        bool pass = memory_type_from_properties(mem_reqs.memoryTypeBits,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &alloc_info.memoryTypeIndex);

        VkDeviceMemory mem;
        res = vkAllocateMemory(m_device, &alloc_info, NULL,
                               &(mem));
        assert(!res);


        //set the content of the memory
        void *data;
        res = vkMapMemory(m_device, mem, 0, mem_reqs.size, 0,
                  (void **)&data);
        assert(!res);

        memcpy(data, &mat, sizeof(mat));

        vkUnmapMemory(m_device, mem);


        //connect the buffer and memory
        res = vkBindBufferMemory(m_device, buf, mem, 0);

    }
    void VKRenderBackEnd::CreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions)
    {

        attributeDescriptions.resize(2);


        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexData, m_pos);



        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexData, m_color);


    }
    void VKRenderBackEnd::CreateVertexBuffer()
    {
        VertexData vertices[] = {
            VertexData(vec3(-0.7, 0.7,  0), vec2(0.0f, 0.0f), vec4(1, 0, 0, 1)),
            VertexData(vec3(0.7, 0.7,  0), vec2(0.0f, 0.0f), vec4(0, 0, 1, 1)),
            VertexData(vec3(0.0, -0.7,  0), vec2(0.0f, 0.0f), vec4(0, 1, 0, 1)),
            //VertexData(vec3(-0.5, 0.5,  0), vec2(0.0f, 0.0f), vec4(0, 1, 0, 1)),
        };



        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vertices[0]) * ARRAY_SIZE_IN_ELEMENTS(vertices);
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        
        vkCreateBuffer(m_device, &bufferInfo, nullptr, &vertexBuffer);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_device, vertexBuffer, &memRequirements);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.memoryTypeIndex = 0;

        alloc_info.allocationSize = memRequirements.size;
        bool pass = memory_type_from_properties(memRequirements.memoryTypeBits,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &alloc_info.memoryTypeIndex);
        VkDeviceMemory mem;
        vkAllocateMemory(m_device, &alloc_info, NULL,
                               &(mem));

        void* data;
        vkMapMemory(m_device, mem, 0, bufferInfo.size, 0, &data);
            memcpy(data, vertices, (size_t) bufferInfo.size);
        vkUnmapMemory(m_device, mem);
        vkBindBufferMemory(m_device, vertexBuffer, mem, 0);
    }
    void VKRenderBackEnd::CreateIndexBuffer()
    {
        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(uint16_t) * indices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        vkCreateBuffer(m_device, &bufferInfo, nullptr, &indexBuffer);



        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_device, indexBuffer, &memRequirements);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.memoryTypeIndex = 0;

        alloc_info.allocationSize = memRequirements.size;
        bool pass = memory_type_from_properties(memRequirements.memoryTypeBits,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &alloc_info.memoryTypeIndex);
        VkDeviceMemory mem;
        vkAllocateMemory(m_device, &alloc_info, NULL,
                               &(mem));

        void* data;
        vkMapMemory(m_device, mem, 0, bufferInfo.size, 0, &data);
            memcpy(data, indices.data(), (size_t) bufferInfo.size);
        vkUnmapMemory(m_device, mem);
        vkBindBufferMemory(m_device, indexBuffer, mem, 0);

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
        CreateRenderPass();
        CreateFramebuffer();
        CreateShaders();
        CreateUiniform();
        CreatePipeline();
        RecordCommandBuffers();
    }

    void VKRenderBackEnd::RenderScene()
    {
        unsigned ImageIndex = 0;
    
        VkResult res = vkAcquireNextImageKHR(m_device, m_swapChainKHR, UINT64_MAX, NULL, NULL, &ImageIndex);
        CHECK_VULKAN_ERROR("vkAcquireNextImageKHR error %d\n" , res);
   
        VkSubmitInfo submitInfo = {};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &m_cmdBufs[ImageIndex];
    
        res = vkQueueSubmit(m_queue, 1, &submitInfo, NULL);    
        CHECK_VULKAN_ERROR("vkQueueSubmit error %d\n", res);
    
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &m_swapChainKHR;
        presentInfo.pImageIndices      = &ImageIndex;
    
        res = vkQueuePresentKHR(m_queue, &presentInfo);    
        CHECK_VULKAN_ERROR("vkQueuePresentKHR error %d\n" , res);
    }

}