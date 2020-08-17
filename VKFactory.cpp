#include "VKPCH.h"
#ifdef DEVELOPER_VERSION
bool GDebugRender;
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*) 
{

	switch (message_severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		BEAR_PRINTF(TEXT("VulkanWarning:" BEAR_PRINT_STR_CURRENT), callback_data->pMessage);
		DebugBreak();
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		BEAR_PRINTF(TEXT("VulkanError:" BEAR_PRINT_STR_CURRENT), callback_data->pMessage);
		BEAR_ASSERT(0);
		break;
	default:
		break;
	}
	return VK_FALSE;
}
#endif

bool VKFactory::LoadFunctions()
{
#define REGISTRATION_INSTANCE(name) 
#define REGISTRATION_DEVICE(name)
#define REGISTRATION(name)  name = BearManagerDynamicLibraries::GetFunctionInProject<PFN_##name>(TEXT("vulkan-1"),#name);if(!name)return false;
#include "VKImports.h"
	return true;
}

bool VKFactory::CreateInstance()
{
	VkApplicationInfo ApplicationInfo = {};
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pNext = NULL;
	ApplicationInfo.pApplicationName = "BearEngine";
	ApplicationInfo.applicationVersion = 1;
	ApplicationInfo.pEngineName = "BearGraphics";
	ApplicationInfo.engineVersion = 1;
#ifdef VK_11
	ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);
#else
	ApplicationInfo.apiVersion = VK_API_VERSION_1_0;
#endif
	BearVector<const char*>InstanceExtensions;
	InstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	InstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#ifdef DEVELOPER_VERSION
	InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	InstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	VkInstanceCreateInfo InstanceCreateInfo = {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = NULL;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledExtensionCount = InstanceExtensions.size();
	InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

#ifdef DEVELOPER_VERSION
	BearVector<const char*>  Layers;
	VkLayerProperties* LayerProperties = nullptr;
	if (GDebugRender)
	{
		{
			uint32_t LayerCount = 0;
			vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
			LayerProperties = bear_alloc< VkLayerProperties>(LayerCount);
			vkEnumerateInstanceLayerProperties(&LayerCount, LayerProperties);
			for (size_t i = 0; i < LayerCount; i++)
			{

				if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_standard_validation"))Layers.push_back(LayerProperties[i].layerName);
				if (strstr(LayerProperties[i].layerName, "VK_LAYER_KHRONOS_validation"))Layers.push_back(LayerProperties[i].layerName);
				if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_parameter_validation"))Layers.push_back(LayerProperties[i].layerName);
				if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_object_tracker"))Layers.push_back(LayerProperties[i].layerName);
				if (strstr(LayerProperties[i].layerName, "VK_LAYER_LUNARG_core_validation"))Layers.push_back(LayerProperties[i].layerName);
				/*else if (BearString::Find(LayerProperties[i].layerName, "VK_LAYER_RENDERDOC_Capture"))
					Layers.push_back(LayerProperties[i].layerName);*/
			}
		}
		
		InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Layers.size());
		InstanceCreateInfo.ppEnabledLayerNames = Layers.data();

		VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo;
		DebugCreateInfo = {};
		DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		DebugCreateInfo.pfnUserCallback = DebugCallback;
		InstanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateInfo;
	}
#endif

	VkResult Result = VK_SUCCESS;

	Result = vkCreateInstance(&InstanceCreateInfo, NULL, &Instance);

#ifdef DEVELOPER_VERSION
	if (LayerProperties)
	{
		bear_free(LayerProperties);
	}
#endif

	if (Result == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		Instance = 0;
		return false;
	}
	else if (Result)
	{
		Instance = 0;
		return false;
	}


	{
#define REGISTRATION_DEVICE(name)
#define REGISTRATION(name)
#define REGISTRATION_INSTANCE(name) name = (PFN_##name)vkGetInstanceProcAddr(Instance,#name);BEAR_CHECK(name);
#include "VKImports.h"
	}

	return true;
}

bool VKFactory::CreateDevice()
{
	VkResult Result = VK_SUCCESS;
	VkDeviceQueueCreateInfo QueueCreateInfo = {};
	if (!CreateGPU(QueueCreateInfo.queueFamilyIndex))
		return false;
	static float QueuePriorities[] = { 0 };
	QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	QueueCreateInfo.pNext = NULL;
	QueueCreateInfo.queueCount = 1;
	QueueCreateInfo.pQueuePriorities = QueuePriorities;
	BearVector<const char*> DeviceExtensions;
	{
		
		BearVector<VkExtensionProperties> AvailableExtensions;

		auto FindExtensionProperty = [&AvailableExtensions](const char* name)->bool {for (auto& i : AvailableExtensions) { if (BearString::Compare(i.extensionName, name) == 0)return true; }return false; };
		{
			uint32_t ExtensionCount = 0;
			Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
			if (Result != VK_SUCCESS)return false;
			AvailableExtensions.resize(0);
			Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, AvailableExtensions.data());
			if (Result != VK_SUCCESS)return false;
		}
		if (!FindExtensionProperty(VK_KHR_SWAPCHAIN_EXTENSION_NAME))return false;
		DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		if (!FindExtensionProperty(VK_KHR_MAINTENANCE1_EXTENSION_NAME))return false;
		DeviceExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
#ifdef RTX
		if (FindExtensionProperty(VK_NV_RAY_TRACING_EXTENSION_NAME))
		{
			DeviceExtensions.push_back(VK_NV_RAY_TRACING_EXTENSION_NAME);
			bSupportRayTracing = true;
		}
#endif
	}
	


	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = NULL;
	DeviceCreateInfo.queueCreateInfoCount = 1;
	DeviceCreateInfo.pQueueCreateInfos = &QueueCreateInfo;
	DeviceCreateInfo.enabledExtensionCount = DeviceExtensions.size();
	DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = NULL;
	DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;

	Result = vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, NULL, &Device);
	if (Result != VK_SUCCESS) 
	{ 
		Device = 0;
		return false;
	}

	{
#define REGISTRATION_DEVICE(name)name = (PFN_##name)vkGetDeviceProcAddr(Device,#name);BEAR_CHECK(name);
#define REGISTRATION(name)
#define REGISTRATION_INSTANCE(name) 
#include "VKImports.h"
	}
#ifdef DEVELOPER_VERSION
	if (GDebugRender)
	{
		VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo;
		DebugCreateInfo = {};
		DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		DebugCreateInfo.pfnUserCallback = DebugCallback;
		V_CHK(vkCreateDebugUtilsMessengerEXT(Instance, &DebugCreateInfo, nullptr, &DebugMessenger));
	}
#endif
	{
		vkGetDeviceQueue(Device, QueueFamilyIndex, 0, &Queue);
	}

	return true;
}

bool VKFactory::CreateGPU(uint32_t& queue_family_index)
{
	VkResult Result = VK_SUCCESS;

	BearVector<VkPhysicalDevice> PhysicalDevices;
	{
		uint32_t GpuCount = 0;

		Result = vkEnumeratePhysicalDevices(Instance, &GpuCount, NULL);
		if (Result != VK_SUCCESS)return false;
		if (GpuCount == 0)return;

		PhysicalDevices.resize(GpuCount);

		Result = vkEnumeratePhysicalDevices(Instance, &GpuCount, PhysicalDevices.data());
		if (Result != VK_SUCCESS)return false;
	}

	for (VkPhysicalDevice i : PhysicalDevices)
	{
		VkPhysicalDeviceProperties Properties = {};
		vkGetPhysicalDeviceProperties(i, &Properties);

		if (Properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)continue;



		BearVector< VkQueueFamilyProperties> QueueProps;
		{
			uint32_t QueueFamilyCount = 0;

			vkGetPhysicalDeviceQueueFamilyProperties(i, &QueueFamilyCount, NULL);
			if (!QueueFamilyCount)continue;

			QueueProps.resize(QueueFamilyCount);

			vkGetPhysicalDeviceQueueFamilyProperties(i, &QueueFamilyCount, QueueProps.data());
			if (QueueFamilyCount < 0)continue;
		}

		for (bsize a=0; a< QueueProps.size() ;a++)
		{
			if (QueueProps[a].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				PhysicalDevice = i;
				queue_family_index = a;

				vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
				vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
				vkGetPhysicalDeviceFeatures(PhysicalDevice, &DeviceFeatures);
				return true;
			}
		}
	}
	return false;
}

VKFactory::VKFactory() :Instance(0), PhysicalDevice(0), Device(0), m_CommandPool(0)
{
	LoadFunctions();
#ifdef DEVELOPER_VERSION
	{
		GDebugRender = BearString::Find(GetCommandLine(), TEXT("-debugrender"));
		if (!GDebugRender)
			GDebugRender = BearString::Find(GetCommandLine(), TEXT("-drender"));
#if defined(_DEBUG)
		GDebugRender = true;
#endif
	}
#endif
	if (!CreateInstance())return;
	if (!CreateDevice())return;

	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = NULL;
		CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndex;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		V_CHK(vkCreateCommandPool(Device, &CommandPoolCreateInfo, NULL, &m_CommandPool));


		VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.pNext = NULL;
		CommandBufferAllocateInfo.commandPool = m_CommandPool;
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;

		V_CHK(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &CommandBuffer));
		//	vkFreeCommandBuffers
	}


	{
		V_CHK(vkWaitForFences(Device, 1, &Fence, true, UINT64_MAX));
		V_CHK(vkResetFences(Device, 1, &Fence));
	}
#ifdef RTX
	if (bSupportRayTracing)
	{
		VkPhysicalDeviceProperties2 Properties;
		bear_fill(PhysicalDeviceRayTracingProperties);
		PhysicalDeviceRayTracingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;
		PhysicalDeviceRayTracingProperties.pNext = VK_NULL_HANDLE;
		Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		Properties.pNext = &PhysicalDeviceRayTracingProperties;
		vkGetPhysicalDeviceProperties2(PhysicalDevice, &Properties);
	}
	BEAR_ASSERT(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DxcCompiler))));
	BEAR_ASSERT(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&DxcLibrary))));
#endif

	/*	{
			VkPipelineCacheCreateInfo PipelineCache;
			PipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			PipelineCache.pNext = NULL;
			PipelineCache.initialDataSize = 0;
			PipelineCache.pInitialData = NULL;
			PipelineCache.flags = 0;
			V_CHK(vkCreatePipelineCache(Device, &PipelineCache, 0, &PipelineCacheDefault));
		}*/
}

VKFactory::~VKFactory()
{
	{
#ifdef RTX
		DxcCompiler->Release();
		DxcLibrary->Release();
#endif
	}
	if(m_CommandPool)
	{
		vkDestroySemaphore(Factory->Device, SemaphoreWait, 0);
		vkDestroyFence(Factory->Device, Fence, 0);
		VkCommandBuffer cmd_bufs[1] = { CommandBuffer };
		vkFreeCommandBuffers(Device, m_CommandPool, 1, cmd_bufs);
		vkDestroyCommandPool(Device, m_CommandPool, NULL);
	}
#ifdef DEVELOPER_VERSION
	if (GDebugRender)
	{
		vkDestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
	}
#endif
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

BearRHI::BearRHIRayTracingBottomLevel* VKFactory::CreateRayTracingBottomLevel(const BearRayTracingBottomLevelDescription& Description)
{
#ifdef RTX
	return bear_new<VKBottomLevel>(Description);
#else 
	return nullptr;
#endif
}

BearRHI::BearRHIRayTracingTopLevel* VKFactory::CreateRayTracingTopLevel(const BearRayTracingTopLevelDescription& Description)
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
	return  bear_new<VKSamplerState>(Description);
}

BearRHI::BearRHIRenderPass* VKFactory::CreateRenderPass(const BearRenderPassDescription& Description)
{
	return  bear_new<VKRenderPass>(Description);
}

BearRHI::BearRHIFrameBuffer* VKFactory::CreateFrameBuffer(const BearFrameBufferDescription& Description)
{
	return  bear_new<VKFrameBuffer>(Description);
}

VkSamplerAddressMode VKFactory::Translation(BearSamplerAddressMode format)
{
	switch (format)
	{
	case BearSamplerAddressMode::Wrap:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case BearSamplerAddressMode::Mirror:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;
	case BearSamplerAddressMode::Clamp:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	case BearSamplerAddressMode::Border:
		return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

VkCullModeFlagBits VKFactory::Translation(BearRasterizerCullMode format)
{
	switch (format)
	{
	case BearRasterizerCullMode::None:
		return VK_CULL_MODE_NONE;
		break;
	case BearRasterizerCullMode::Front:
		return VK_CULL_MODE_FRONT_BIT;
		break;
	case BearRasterizerCullMode::Back:
		return VK_CULL_MODE_BACK_BIT;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_CULL_MODE_NONE;
}

VkPolygonMode VKFactory::Translation(BearRasterizerFillMode format)
{
	switch (format)
	{
	case BearRasterizerFillMode::Wireframe:
		return VK_POLYGON_MODE_LINE;
		break;
	case BearRasterizerFillMode::Solid:
		return VK_POLYGON_MODE_FILL;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_POLYGON_MODE_FILL;

}

VkFrontFace VKFactory::Translation(BearRasterizerFrontFace format)
{
	switch (format)
	{
	case BearRasterizerFrontFace::CounterClockwise:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	case BearRasterizerFrontFace::FaceClockwise:
		return VK_FRONT_FACE_CLOCKWISE;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_FRONT_FACE_CLOCKWISE;
}

VkBlendFactor VKFactory::Translation(BearBlendFactor format)
{
	switch (format)
	{
	case BearBlendFactor::Zero:
		return VK_BLEND_FACTOR_ZERO;
		break;
	case BearBlendFactor::One:
		return VK_BLEND_FACTOR_ONE;
		break;
	case BearBlendFactor::SrcColor:
		return VK_BLEND_FACTOR_SRC_COLOR;
		break;
	case BearBlendFactor::InvSrcColor:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		break;
	case BearBlendFactor::SrcAlpha:
		return VK_BLEND_FACTOR_SRC_ALPHA;
		break;
	case BearBlendFactor::InvSrcAlpha:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		break;
	case BearBlendFactor::DestAlpha:
		return VK_BLEND_FACTOR_DST_ALPHA;
		break;
	case BearBlendFactor::InvDestAlpha:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		break;
	case BearBlendFactor::DestColor:
		return VK_BLEND_FACTOR_DST_COLOR;
		break;
	case BearBlendFactor::InvDestColor:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		break;
	case BearBlendFactor::BlendFactor:
		return VK_BLEND_FACTOR_CONSTANT_COLOR;
		break;
	case BearBlendFactor::InvBlendFactor:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_BLEND_FACTOR_ZERO;
}

VkBlendOp VKFactory::Translation(BearBlendOp format)
{
	switch (format)
	{
	case BearBlendOp::Add:
		return VK_BLEND_OP_ADD;
		break;
	case BearBlendOp::Subtract:
		VK_BLEND_OP_SUBTRACT;
		break;
	case BearBlendOp::RevSubtract:
		VK_BLEND_OP_REVERSE_SUBTRACT;
		break;
	case BearBlendOp::Min:
		VK_BLEND_OP_MIN;
		break;
	case BearBlendOp::Max:
		VK_BLEND_OP_MAX;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_BLEND_OP_ADD;
}

VkCompareOp VKFactory::Translation(BearCompareFunction format)
{
	switch (format)
	{
	case BearCompareFunction::Never:
		return VK_COMPARE_OP_NEVER;
		break;
	case BearCompareFunction::Always:
		return VK_COMPARE_OP_ALWAYS;
		break;
	case BearCompareFunction::Equal:
		return VK_COMPARE_OP_EQUAL;
		break;
	case BearCompareFunction::NotEqual:
		return VK_COMPARE_OP_NOT_EQUAL;
		break;
	case BearCompareFunction::Less:
		return VK_COMPARE_OP_LESS;
		break;
	case BearCompareFunction::Greater:
		return VK_COMPARE_OP_GREATER;
		break;
	case BearCompareFunction::LessEqual:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
		break;
	case BearCompareFunction::GreaterEqual:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
		break;
	default:
		BEAR_CHECK(0);
		break;
	}
	return VK_COMPARE_OP_NEVER;
}

VkStencilOp VKFactory::Translation(BearStencilOp format)
{
	switch (format)
	{
	case BearStencilOp::Keep:
		return VK_STENCIL_OP_KEEP;
		break;
	case BearStencilOp::Zero:
		return VK_STENCIL_OP_ZERO;
		break;
	case BearStencilOp::Replace:
		return VK_STENCIL_OP_REPLACE;
		break;
	case BearStencilOp::IncrSat:
		return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		break;
	case BearStencilOp::DecrSat:
		return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		break;
	case BearStencilOp::Invert:
		return VK_STENCIL_OP_INVERT;
		break;
	case BearStencilOp::Incr:
		return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		break;
	case BearStencilOp::Decr:
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
	case BearTexturePixelFormat::None:
		BEAR_CHECK(0);
		break;
	case BearTexturePixelFormat::R8:
		return VK_FORMAT_R8_UNORM;
	case BearTexturePixelFormat::R8G8:
		return VK_FORMAT_R8G8_UNORM;
	case BearTexturePixelFormat::R8G8B8:
		return VK_FORMAT_R8G8B8_UNORM;
	case BearTexturePixelFormat::R8G8B8A8:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case BearTexturePixelFormat::R32F:
		return VK_FORMAT_R32_SFLOAT;
	case BearTexturePixelFormat::R32G32F:
		return VK_FORMAT_R32G32_SFLOAT;
	case BearTexturePixelFormat::R32G32B32F:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case BearTexturePixelFormat::R32G32B32A32F:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case BearTexturePixelFormat::BC1:
		return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case BearTexturePixelFormat::BC1a:
		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case BearTexturePixelFormat::BC2:
		return VK_FORMAT_BC2_UNORM_BLOCK;
	case BearTexturePixelFormat::BC3:
		return VK_FORMAT_BC3_UNORM_BLOCK;
		break;
	case BearTexturePixelFormat::BC4:
		return VK_FORMAT_BC4_UNORM_BLOCK;
	case BearTexturePixelFormat::BC5:
		return VK_FORMAT_BC5_UNORM_BLOCK;
	case BearTexturePixelFormat::BC6:
		return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case BearTexturePixelFormat::BC7:
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
	case BearRenderTargetFormat::None:
		break;
	case BearRenderTargetFormat::R8:
		return VK_FORMAT_R8_UNORM;
		break;
	case BearRenderTargetFormat::R8G8:
		return VK_FORMAT_R8G8_UNORM;
		break;
	case BearRenderTargetFormat::R8G8B8A8:
		return VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case BearRenderTargetFormat::B8G8R8A8:
		return VK_FORMAT_B8G8R8A8_UNORM;
		break;
	case BearRenderTargetFormat::R32F:
		return VK_FORMAT_R32_SFLOAT;
		break;
	case BearRenderTargetFormat::R32G32F:
		return VK_FORMAT_R32G32_SFLOAT;
		break;
	case BearRenderTargetFormat::R32G32B32F:
		return VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case BearRenderTargetFormat::R32G32B32A32F:
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
	case BearDepthStencilFormat::Depth16:
		return VK_FORMAT_D16_UNORM;
		break;
	case BearDepthStencilFormat::Depth32F:
		return VK_FORMAT_D32_SFLOAT;
		break;
	case BearDepthStencilFormat::Depth24Stencil8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
		break;
	case BearDepthStencilFormat::Depth32FStencil8:
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
#ifdef RTX
	return bSupportRayTracing;
#else
	return false;
#endif
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

void VKFactory::UnlockCommandBuffer()
{
	static VkPipelineStageFlags StageOut = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	V_CHK(vkEndCommandBuffer(CommandBuffer));
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = 0;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = 0;// &buf.acquire_semaphore;
	SubmitInfo.pWaitDstStageMask = &StageOut;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	SubmitInfo.commandBufferCount = 1;
	V_CHK(vkQueueSubmit(Factory->Queue, 1, &SubmitInfo, Fence));
	V_CHK(vkWaitForFences(Factory->Device, 1, &Fence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &Fence));
	m_CommandMutex.Unlock();
}

