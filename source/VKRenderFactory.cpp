#include "vulkanPCH.h"
static const char* InstanceExtensions[] =
{
#ifdef DEBUG
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
};
static const char* DeviceExtensions[] =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
static const char* ValidationLayers[] =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef DEBUG
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
	BearLog::Printf(TEXT("vulkan:" BEAR_PRINT_ANSI), pCallbackData->pMessage);
	return VK_FALSE;
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
#endif
VKRenderFactory::VKRenderFactory():Instance(0), PhysicalDevice(0), Device(0)
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = "BearProgram";
	app_info.applicationVersion = 1;
	app_info.pEngineName = "BearGraphics";
	app_info.engineVersion = 1;
	app_info.apiVersion = VK_API_VERSION_1_0;

	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pNext = NULL;
	inst_info.flags = 0;
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledExtensionCount = sizeof(InstanceExtensions)/sizeof(const char*);
	inst_info.ppEnabledExtensionNames = InstanceExtensions;
#ifdef DEBUG
	inst_info.enabledLayerCount = 0;// sizeof(ValidationLayers) / sizeof(const char*);
	inst_info.ppEnabledLayerNames = ValidationLayers;
	VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo;
	DebugCreateInfo = {};
	DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugCreateInfo.pfnUserCallback = debugCallback;
	inst_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateInfo;
#else
	inst_info.enabledLayerCount = 0;
	inst_info.ppEnabledLayerNames = 0;
#endif
	VkResult res;

	res = vkCreateInstance(&inst_info, NULL, &Instance);
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
		Instance = 0;
		return;
	}
	else if (res) {
		Instance = 0;
		return;
	}

	uint32_t GpuCount = 1, QueueFamilyCount;
	 res = vkEnumeratePhysicalDevices(Instance, &GpuCount, NULL);
	if (GpuCount == 0)return;

	res = vkEnumeratePhysicalDevices(Instance, &GpuCount, &PhysicalDevice);
	if (res || GpuCount ==0)return;


	VkDeviceQueueCreateInfo queue_info = {};

	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, NULL);
	if (!QueueFamilyCount)return;

	BearVector< VkQueueFamilyProperties> QueueProps;
	QueueProps.resize(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, &QueueProps[0]);
	if (QueueFamilyCount < 0)return;

	bool found = false;
	for (uint32_t  i = 0; i < QueueFamilyCount; i++)
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
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledExtensionCount = sizeof(DeviceExtensions) / sizeof(const char*);;
	device_info.ppEnabledExtensionNames = DeviceExtensions;
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = NULL;
	device_info.pEnabledFeatures = NULL;

	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);

	res = vkCreateDevice(PhysicalDevice, &device_info, NULL, &Device);
	if (res != VK_SUCCESS) { Device = 0; }
	else
	{
#ifdef DEBUG 
		V_CHK(CreateDebugUtilsMessengerEXT(Instance, &DebugCreateInfo, nullptr, &DebugMessenger));
#endif
		vkGetDeviceQueue(Device, QueueFamilyIndex, 0, &Queue);
	}
}

VKRenderFactory::~VKRenderFactory()
{
#ifdef DEBUG 
	DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
#endif
	
	if (Device)
		vkDestroyDevice(Device, NULL);
	if(Instance)
		vkDestroyInstance(Instance, NULL);
}

BearRenderBase::BearRenderInterfaceBase * VKRenderFactory::CreateInterface()
{
	return nullptr;
}

BearRenderBase::BearRenderContextBase * VKRenderFactory::CreateContext()
{
	return bear_new<VKRenderContext>(); 
}

BearRenderBase::BearRenderViewportBase * VKRenderFactory::CreateViewport(void * Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync, const BearGraphics::BearRenderViewportDescription&Description)
{
	return bear_new<VKRenderViewport>(Handle,Width,Height,Fullscreen,VSync, Description);
}

BearRenderBase::BearRenderShaderBase * VKRenderFactory::CreateShader(BearGraphics::BearShaderType Type)
{
	return nullptr;
}

BearRenderBase::BearRenderIndexBufferBase * VKRenderFactory::CreateIndexBuffer()
{
	return bear_new<VKRenderIndexBuffer>();
}

BearRenderBase::BearRenderVertexBufferBase * VKRenderFactory::CreateVertexBuffer()
{
	return bear_new<VKRenderVertexBuffer>();
}
