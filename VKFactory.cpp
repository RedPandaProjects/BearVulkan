#include "VKPCH.h"
static const char* InstanceExtensions[] =
{

	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
};
static const char* DeviceExtensions[] =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	VK_NV_RAY_TRACING_EXTENSION_NAME,
};
PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;




#define REGISTRATION(name) PFN_##name name = VK_NULL_HANDLE;
#include "VKImports.h"

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		BEAR_PRINTF(TEXT("VulkanWarning:" BEAR_PRINT_STR_CURRENT), pCallbackData->pMessage);
		DebugBreak();
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		BEAR_PRINTF(TEXT("VulkanError:" BEAR_PRINT_STR_CURRENT), pCallbackData->pMessage);
		BEAR_ASSERT(0);
		break;
	default:
		break;
	}
	
	return VK_FALSE;
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
bool GDebugRender;
VKFactory::VKFactory() :Instance(0), PhysicalDevice(0), Device(0), PipelineCacheDefault(0), PipelineLayout(0), m_CommandPool(0),DefaultSampler(0), RenderPass(0)
{
	LoadFunctions();
	uint32_t LayerCount = 0;
	VkLayerProperties* LayerProperties=0;
	BearVector<const char*>  Layers;

	GDebugRender = BearString::Find(GetCommandLine(), TEXT("-debugrender"));
	if (!GDebugRender)
		GDebugRender = BearString::Find(GetCommandLine(), TEXT("-drender"));
#if defined(_DEBUG)
	GDebugRender = true;
#endif
	if (GDebugRender)
	{
		vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
		LayerProperties = bear_alloc< VkLayerProperties>(LayerCount); 
		vkEnumerateInstanceLayerProperties(&LayerCount, LayerProperties);
		for (size_t i = 0; i < LayerCount; i++)
		{
			; ; ; 
			 if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_standard_validation"))Layers.push_back(LayerProperties[i].layerName);
			if (strstr(LayerProperties[i].layerName, "VK_LAYER_KHRONOS_validation"))Layers.push_back(LayerProperties[i].layerName);
			if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_parameter_validation"))Layers.push_back(LayerProperties[i].layerName);
			if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_object_tracker"))Layers.push_back(LayerProperties[i].layerName);
			if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_core_validation"))Layers.push_back(LayerProperties[i].layerName);
			/*else if (BearString::Find(LayerProperties[i].layerName, "VK_LAYER_RENDERDOC_Capture"))
				Layers.push_back(LayerProperties[i].layerName);*/
		}
		
	}
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = "BearProgram";
	app_info.applicationVersion = 1;
	app_info.pEngineName = "BearGraphics";
	app_info.engineVersion = 1;
#ifdef VK_11
	app_info.apiVersion = VK_MAKE_VERSION(1, 1, 0);
#else
	app_info.apiVersion = VK_API_VERSION_1_0;
	
#endif
	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pNext = NULL;
	inst_info.flags = 0;
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledExtensionCount = sizeof(InstanceExtensions) / sizeof(const char*);
	inst_info.ppEnabledExtensionNames = InstanceExtensions;
	VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo;
	if (GDebugRender)
	{
		
		inst_info.enabledLayerCount = static_cast<uint32_t>(Layers.size());
		inst_info.ppEnabledLayerNames = Layers.data();


	
		DebugCreateInfo = {};
		DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |

			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		DebugCreateInfo.pfnUserCallback = debugCallback;
		inst_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateInfo;
	}
	else
	{
		inst_info.enabledExtensionCount -= 2;
		inst_info.enabledLayerCount = 0;
		inst_info.ppEnabledLayerNames = 0;
	}
	VkResult res;

	res = vkCreateInstance(&inst_info, NULL, &Instance);
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		bear_free(LayerProperties);
		Instance = 0;
		return;
	}
	else if (res) 
	{
		bear_free(LayerProperties);
		Instance = 0;
		return;
	}
	if (LayerProperties)bear_free(LayerProperties);

	uint32_t GpuCount = 1, QueueFamilyCount;
	res = vkEnumeratePhysicalDevices(Instance, &GpuCount, NULL);
	if (GpuCount == 0)return;

	res = vkEnumeratePhysicalDevices(Instance, &GpuCount, &PhysicalDevice);
	if (res || GpuCount == 0)return;
	{
		vkGetPhysicalDeviceFeatures(PhysicalDevice, &DeviceFeatures);
	}

	VkDeviceQueueCreateInfo queue_info = {};

	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, NULL);
	if (!QueueFamilyCount)return;

	BearVector< VkQueueFamilyProperties> QueueProps;
	QueueProps.resize(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, &QueueProps[0]);
	if (QueueFamilyCount < 0)return;

	bool found = false;
	for (uint32_t i = 0; i < QueueFamilyCount; i++)
	{
		if (QueueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queue_info.queueFamilyIndex = i;
			found = true;
			break;
		}
	}
	if (!found)return;
	QueueFamilyIndex = queue_info.queueFamilyIndex;
	float queue_priorities[1] = { 0.0 };
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = NULL;
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = queue_priorities;

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = NULL;
#ifdef DEBUG
	//device_info.pNext = &DeviceFeatures;
#endif
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledExtensionCount = sizeof(DeviceExtensions) / sizeof(const char*);;
	device_info.ppEnabledExtensionNames = DeviceExtensions;
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = NULL;
	device_info.pEnabledFeatures = &DeviceFeatures;

	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);

	res = vkCreateDevice(PhysicalDevice, &device_info, NULL, &Device);
	if (res != VK_SUCCESS) { Device = 0; }
	else
	{
		{
#define REGISTRATION_DEVICE(name)name = (PFN_##name)vkGetDeviceProcAddr(Device,#name);BEAR_CHECK(name);
#define REGISTRATION(name)
#include "VKImports.h"
		}
		if (GDebugRender)
		{
			V_CHK(CreateDebugUtilsMessengerEXT(Instance, &DebugCreateInfo, nullptr, &DebugMessenger));
		}
	
		vkGetDeviceQueue(Device, QueueFamilyIndex, 0, &Queue);
		{
			VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
			PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			PipelineLayoutCreateInfo.pNext = NULL;
			PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
			PipelineLayoutCreateInfo.pPushConstantRanges = NULL;
			PipelineLayoutCreateInfo.setLayoutCount = 0;
			PipelineLayoutCreateInfo.pSetLayouts = 0;

			V_CHK(vkCreatePipelineLayout(Device, &PipelineLayoutCreateInfo, NULL, &PipelineLayout));
		}
		{
			VkPipelineCacheCreateInfo PipelineCache;
			PipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			PipelineCache.pNext = NULL;
			PipelineCache.initialDataSize = 0;
			PipelineCache.pInitialData = NULL;
			PipelineCache.flags = 0;
			V_CHK(vkCreatePipelineCache(Device, &PipelineCache, 0, &PipelineCacheDefault));
		}

		{
			VkCommandPoolCreateInfo cmd_pool_info = {};
			cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmd_pool_info.pNext = NULL;
			cmd_pool_info.queueFamilyIndex = QueueFamilyIndex;
			cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			V_CHK(vkCreateCommandPool(Device, &cmd_pool_info, NULL, &m_CommandPool));


			VkCommandBufferAllocateInfo cmd = {};
			cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmd.pNext = NULL;
			cmd.commandPool = m_CommandPool;
			cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmd.commandBufferCount = 1;

			V_CHK(vkAllocateCommandBuffers(Device, &cmd, &CommandBuffer));
			//	vkFreeCommandBuffers
		}

		{
			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.maxAnisotropy =1;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

			V_CHK(vkCreateSampler(Device, &samplerInfo, nullptr, &DefaultSampler));
		}
		{
			VkAttachmentDescription attachments[2];
			attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			attachments[0].flags = 0;

			VkAttachmentReference color_reference = {};
			color_reference.attachment = 0;
			color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depth_reference = {};
			depth_reference.attachment = 1;
			depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.flags = 0;
			subpass.inputAttachmentCount = 0;
			subpass.pInputAttachments = NULL;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &color_reference;
			subpass.pResolveAttachments = NULL;
			subpass.pDepthStencilAttachment = NULL;
			subpass.preserveAttachmentCount = 0;
			subpass.pPreserveAttachments = NULL;

			VkRenderPassCreateInfo rp_info = {};
			rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			rp_info.pNext = NULL;
			rp_info.attachmentCount = 1;
			rp_info.pAttachments = attachments;
			rp_info.subpassCount = 1;
			rp_info.pSubpasses = &subpass;
			rp_info.dependencyCount = 0;
			rp_info.pDependencies = NULL;

			V_CHK(vkCreateRenderPass(Device, &rp_info, NULL, &RenderPass));

			VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			V_CHK(vkCreateFence(Device, &info, nullptr, &Fence));

			VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
			imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			imageAcquiredSemaphoreCreateInfo.pNext = NULL;
			imageAcquiredSemaphoreCreateInfo.flags = 0;

			V_CHK(vkCreateSemaphore(Device, &imageAcquiredSemaphoreCreateInfo, NULL, &SemaphoreWait));
		}
		{
			V_CHK(vkWaitForFences(Device, 1, &Fence, true, UINT64_MAX));
			V_CHK(vkResetFences(Device, 1, &Fence));
		}
		{
			vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
		}
		{
#ifdef RTX
			VkPhysicalDeviceProperties2 Properties;
			bear_fill(PhysicalDeviceRayTracingProperties);
			PhysicalDeviceRayTracingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;
			PhysicalDeviceRayTracingProperties.pNext = VK_NULL_HANDLE;
			Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			Properties.pNext = &PhysicalDeviceRayTracingProperties;
			vkGetPhysicalDeviceProperties2(PhysicalDevice, &Properties);
#endif
		}
		if (GDebugRender)
		{
			CmdBeginDebugUtilsLabelEXT =
				(PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(
					Instance,
					"vkCmdBeginDebugUtilsLabelEXT");
			CmdEndDebugUtilsLabelEXT =
				(PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(
					Instance,
					"vkCmdEndDebugUtilsLabelEXT");

		}
#ifdef RTX
		BEAR_ASSERT(SUCCEEDED( DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DxcCompiler))));
		BEAR_ASSERT(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&DxcLibrary))));
#endif
	}

}

VKFactory::~VKFactory()
{
	{
#ifdef RTX
		DxcCompiler->Release();
		DxcLibrary->Release();
#endif
	}
	if (RenderPass) {
		vkDestroyRenderPass(Device, RenderPass, 0);
	}
	if (DefaultSampler) { vkDestroySampler(Device, DefaultSampler, 0); }
	if(m_CommandPool)
	{
		vkDestroySemaphore(Factory->Device, SemaphoreWait, 0);
		vkDestroyFence(Factory->Device, Fence, 0);
		VkCommandBuffer cmd_bufs[1] = { CommandBuffer };
		vkFreeCommandBuffers(Device, m_CommandPool, 1, cmd_bufs);
		vkDestroyCommandPool(Device, m_CommandPool, NULL);
	}
	if (GDebugRender)
	{

		DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
	}
	if (PipelineLayout)vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
	if (PipelineCacheDefault)
		vkDestroyPipelineCache(Device, PipelineCacheDefault, nullptr);
	if (Device)
		vkDestroyDevice(Device, nullptr);
	if (Instance)
		vkDestroyInstance(Instance, nullptr);

}

BearRHI::BearRHIContext* VKFactory::CreateContext()
{
	return bear_new<VKContext>();
}

BearRHI::BearRHIViewport* VKFactory::CreateViewport(void* Handle, size_t Width, size_t Height, bool Fullscreen, bool VSync, const BearViewportDescription& Description)
{
	return bear_new<VKViewport>(Handle, Width, Height, Fullscreen, VSync, Description);
}

BearRHI::BearRHIShader* VKFactory::CreateShader(BearShaderType Type)
{
	return bear_new<VKShader>(Type);
}

BearRHI::BearRHIVertexBuffer* VKFactory::CreateVertexBuffer()
{
	return bear_new<VKVertexBuffer>();
}

BearRHI::BearRHIIndexBuffer* VKFactory::CreateIndexBuffer()
{
	return bear_new<VKIndexBuffer>();
}

BearRHI::BearRHIPipelineGraphics* VKFactory::CreatePipelineGraphics(const BearPipelineGraphicsDescription& Description)
{
	return bear_new<VKPipelineGraphics>(Description);
}

BearRHI::BearRHIPipelineMesh* VKFactory::CreatePipelineMesh(const BearPipelineMeshDescription& Description)
{
	return bear_new<VKPipelineMesh>(Description);
}

BearRHI::BearRHIPipelineRayTracing* VKFactory::CreatePipelineRayTracing(const BearPipelineRayTracingDescription& Description)
{
#ifdef RTX
	return bear_new<VKPipelineRayTracing>(Description);
#else 
	return nullptr;
#endif
}

BearRHI::BearRHIBottomLevel* VKFactory::CreateBottomLevel(const BearBottomLevelDescription& Description)
{
#ifdef RTX
	return bear_new<VKBottomLevel>(Description);
#else 
	return nullptr;
#endif
}

BearRHI::BearRHITopLevel* VKFactory::CreateTopLevel(const BearTopLevelDescription& Description)
{
#ifdef RTX
	return bear_new<VKTopLevel>(Description);
#else 
	return nullptr;
#endif
}

BearRHI::BearRHIRayTracingShaderTable* VKFactory::CreateRayTracingShaderTable(const BearRayTracingShaderTableDescription& Description)
{
#ifdef RTX
	return bear_new<VKRayTracingShaderTable>(Description);
#else 
	return nullptr;
#endif
}

BearRHI::BearRHIUniformBuffer* VKFactory::CreateUniformBuffer(size_t Stride, size_t Count, bool Dynamic)
{
	return bear_new<VKUniformBuffer>(Stride, Count, Dynamic);
}


BearRHI::BearRHIRootSignature* VKFactory::CreateRootSignature(const BearRootSignatureDescription& Description)
{
	return bear_new<VKRootSignature>(Description);
}

BearRHI::BearRHIDescriptorHeap* VKFactory::CreateDescriptorHeap(const BearDescriptorHeapDescription& Description)
{
	return bear_new<VKDescriptorHeap>(Description);
}

BearRHI::BearRHITexture2D* VKFactory::CreateTexture2D(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data)
{
	return bear_new<VKTexture2D>(Width,Height,Mips,Count, PixelFormat, TypeUsage,data);
}

BearRHI::BearRHITextureCube* VKFactory::CreateTextureCube(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data)
{
	return bear_new<VKTextureCube>(Width, Height, Mips, Count, PixelFormat, TypeUsage, data);
}

BearRHI::BearRHIStructuredBuffer* VKFactory::CreateStructuredBuffer(size_t size, void* data, bool UAV)
{
	return bear_new<VKStructuredBuffer>(size, data, UAV);
}

BearRHI::BearRHITexture2D* VKFactory::CreateTexture2D(size_t Width, size_t Height, BearRenderTargetFormat Format)
{
	return bear_new<VKTexture2D>(Width, Height, Format);
}

BearRHI::BearRHITexture2D* VKFactory::CreateTexture2D(size_t Width, size_t Height, BearDepthStencilFormat Format)
{
	return bear_new<VKTexture2D>(Width, Height, Format);
}

BearRHI::BearRHISampler* VKFactory::CreateSampler(const BearSamplerDescription& Description)
{
	return  bear_new<VKSamplerState>(Description);;
}

BearRHI::BearRHIRenderPass* VKFactory::CreateRenderPass(const BearRenderPassDescription& Description)
{
	return  bear_new<VKRenderPass>(Description);;;
}

BearRHI::BearRHIFrameBuffer* VKFactory::CreateFrameBuffer(const BearFrameBufferDescription& Description)
{
	return  bear_new<VKFrameBuffer>(Description);;;
}

VkSamplerAddressMode VKFactory::Translation(BearSamplerAddressMode format)
{
	switch (format)
	{
	case SAM_WRAP:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case SAM_MIRROR:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;
	case SAM_CLAMP:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	case SAM_BORDER:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

VkCullModeFlagBits VKFactory::Translate(BearRasterizerCullMode format)
{
	switch (format)
	{
	case RCM_NONE:
		return VK_CULL_MODE_NONE;
		break;
	case RCM_FRONT:
		return VK_CULL_MODE_FRONT_BIT;
		break;
	case RCM_BACK:
		return VK_CULL_MODE_BACK_BIT;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_CULL_MODE_NONE;
}

VkPolygonMode VKFactory::Translate(BearRasterizerFillMode format)
{
	switch (format)
	{
	case RFM_WIREFRAME:
		return VK_POLYGON_MODE_LINE;
		break;
	case RFM_SOLID:
		return VK_POLYGON_MODE_FILL;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_POLYGON_MODE_FILL;

}

VkFrontFace VKFactory::Translate(BearRasterizerFrontFace format)
{
	switch (format)
	{
	case RFF_COUNTER_CLOCKWISE:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	case RFF_FACE_CLOCKWISE:
		return VK_FRONT_FACE_CLOCKWISE;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_FRONT_FACE_CLOCKWISE;
}

VkBlendFactor VKFactory::Translate(BearBlendFactor format)
{
	switch (format)
	{
	case BF_ZERO:
		return VK_BLEND_FACTOR_ZERO;
		break;
	case BF_ONE:
		return VK_BLEND_FACTOR_ONE;
		break;
	case BF_SRC_COLOR:
		return VK_BLEND_FACTOR_SRC_COLOR;
		break;
	case BF_INV_SRC_COLOR:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		break;
	case BF_SRC_ALPHA:
		return VK_BLEND_FACTOR_SRC_ALPHA;
		break;
	case BF_INV_SRC_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		break;
	case BF_DEST_ALPHA:
		return VK_BLEND_FACTOR_DST_ALPHA;
		break;
	case BF_INV_DEST_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		break;
	case BF_DEST_COLOR:
		return VK_BLEND_FACTOR_DST_COLOR;
		break;
	case BF_INV_DEST_COLOR:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		break;
	case BF_BLEND_FACTOR:
		return VK_BLEND_FACTOR_CONSTANT_COLOR;
		break;
	case BF_INV_BLEND_FACTOR:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_BLEND_FACTOR_ZERO;
}

VkBlendOp VKFactory::Translate(BearBlendOp format)
{
	switch (format)
	{
	case BO_ADD:
		return VK_BLEND_OP_ADD;
		break;
	case BO_SUBTRACT:
		VK_BLEND_OP_SUBTRACT;
		break;
	case BO_REV_SUBTRACT:
		VK_BLEND_OP_REVERSE_SUBTRACT;
		break;
	case BO_MIN:
		VK_BLEND_OP_MIN;
		break;
	case BO_MAX:
		VK_BLEND_OP_MAX;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_BLEND_OP_ADD;
}

VkCompareOp VKFactory::Translate(BearCompareFunction format)
{
	switch (format)
	{
	case CF_NEVER:
		return VK_COMPARE_OP_NEVER;
		break;
	case CF_ALWAYS:
		return VK_COMPARE_OP_ALWAYS;
		break;
	case CF_EQUAL:
		return VK_COMPARE_OP_EQUAL;
		break;
	case CF_NOTEQUAL:
		return VK_COMPARE_OP_NOT_EQUAL;
		break;
	case CF_LESS:
		return VK_COMPARE_OP_LESS;
		break;
	case CF_GREATER:
		return VK_COMPARE_OP_GREATER;
		break;
	case CF_LESSEQUAL:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
		break;
	case CF_GREATEREQUAL:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_COMPARE_OP_NEVER;
}

VkStencilOp VKFactory::Translate(BearStencilOp format)
{
	switch (format)
	{
	case SO_KEEP:
		return VK_STENCIL_OP_KEEP;
		break;
	case SO_ZERO:
		return VK_STENCIL_OP_ZERO;
		break;
	case SO_REPLACE:
		return VK_STENCIL_OP_REPLACE;
		break;
	case SO_INCR_SAT:
		return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		break;
	case SO_DECR_SAT:
		return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		break;
	case SO_INVERT:
		return VK_STENCIL_OP_INVERT;
		break;
	case SO_INCR:
		return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		break;
	case SO_DECR:
		return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_STENCIL_OP_KEEP;
}

VkFormat VKFactory::Translation(BearTexturePixelFormat format)
{
	switch (format)
	{
	case TPF_NONE:
		BEAR_CHECK(0);
		break;
	case TPF_R8:
		return VK_FORMAT_R8_UNORM;
	case TPF_R8G8:
		return VK_FORMAT_R8G8_UNORM;
	case TPF_R8G8B8:
		return VK_FORMAT_R8G8B8_UNORM;
	case TPF_R8G8B8A8:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case TPF_R32F:
		return VK_FORMAT_R32_SFLOAT;
	case TPF_R32G32F:
		return VK_FORMAT_R32G32_SFLOAT;
	case TPF_R32G32B32F:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case TPF_R32G32B32A32F:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case TPF_BC1:
		return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case TPF_BC1a:
		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case TPF_BC2:
		return VK_FORMAT_BC2_UNORM_BLOCK;
	case TPF_BC3:
		return VK_FORMAT_BC3_UNORM_BLOCK;
		break;
	case TPF_BC4:
		return VK_FORMAT_BC4_UNORM_BLOCK;
	case TPF_BC5:
		return VK_FORMAT_BC5_UNORM_BLOCK;
	case TPF_BC6:
		return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case TPF_BC7:
		return VK_FORMAT_BC7_UNORM_BLOCK;
	default:	BEAR_CHECK(0);
		break;
	}
	return VK_FORMAT_UNDEFINED;
}

VkFormat VKFactory::Translation(BearRenderTargetFormat format)
{
	switch (format)
	{
	case RTF_NONE:
		break;
	case RTF_R8:
		return VK_FORMAT_R8_UNORM;
		break;
	case RTF_R8G8:
		return VK_FORMAT_R8G8_UNORM;
		break;
	case RTF_R8G8B8A8:
		return VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case RTF_B8G8R8A8:
		return VK_FORMAT_B8G8R8A8_UNORM;
		break;
	case RTF_R32F:
		return VK_FORMAT_R32_SFLOAT;
		break;
	case RTF_R32G32F:
		return VK_FORMAT_R32G32_SFLOAT;
		break;
	case RTF_R32G32B32F:
		return VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case RTF_R32G32B32A32F:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
		break;
	default:
		BEAR_CHECK(0);
		break;
	};
	return VK_FORMAT_R32G32B32A32_SFLOAT;
}

VkFormat VKFactory::Translation(BearDepthStencilFormat format)
{
	switch (format)
	{
	case DSF_DEPTH16:
		return VK_FORMAT_D16_UNORM;
		break;
	case DSF_DEPTH32F:
		return VK_FORMAT_D32_SFLOAT;
		break;
	case DSF_DEPTH24_STENCIL8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
		break;
	case DSF_DEPTH32F_STENCIL8:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_FORMAT_D32_SFLOAT_S8_UINT;
}

bool VKFactory::SupportRayTracing()
{
	return false;
}

bool VKFactory::SupportMeshShader()
{
	return false;
}

void VKFactory::LockCommandBuffer()
{
	m_CommandMutex.Lock();

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	{
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	}
	V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));
}

void VKFactory::UnlockCommandBuffer(const VkSemaphore* pSignalSemaphores)
{
	static VkPipelineStageFlags StageOut = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	V_CHK(vkEndCommandBuffer(CommandBuffer));
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = 0;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = 0;// &buf.acquire_semaphore;
	submit_info.pWaitDstStageMask = &StageOut;
	submit_info.pCommandBuffers = &CommandBuffer;
	submit_info.commandBufferCount = 1;
	V_CHK(vkQueueSubmit(Factory->Queue, 1, &submit_info, Fence));
	V_CHK(vkWaitForFences(Factory->Device, 1, &Fence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &Fence));
	m_CommandMutex.Unlock();
}

void VKFactory::LoadFunctions()
{
#define REGISTRATION_DEVICE(name)
#define REGISTRATION(name)  name = BearManagerDynamicLibraries::GetFunctionInProject<PFN_##name>(TEXT("vulkan-1"),#name);BEAR_CHECK(name);
#include "VKImports.h"
}

