#include "VKPCH.h"
VKFactory* Factory;
bool RHIInitialize()
{
	 Factory = bear_new<VKFactory>();
	if (!Factory->Empty())
	{
		GFactory = Factory;
		GStats = bear_new<VKStats>();
		BEAR_ASSERT(GFactory);
		return true;
	}
	bear_delete(Factory);
	GFactory = 0;
	return false;
}
void vk_error(VkResult result)
{
    const bchar* text = TEXT("UNKOWN");
	switch (result)
	{
    case VK_SUCCESS:
        return;
    case VK_NOT_READY:
        text = TEXT("VK_NOT_READY");
        break;
    case VK_TIMEOUT:
        text = TEXT("VK_TIMEOUT");
        break;
    case VK_EVENT_SET:
        text = TEXT("VK_EVENT_SET");
        break;
    case VK_EVENT_RESET:
        text = TEXT("VK_EVENT_RESET");
        break;
    case VK_INCOMPLETE:
        text = TEXT("VK_INCOMPLETE");
        break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        text = TEXT("VK_ERROR_OUT_OF_HOST_MEMORY");
        break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:

        text = TEXT("VK_ERROR_OUT_OF_DEVICE_MEMORY");
        break;
    case VK_ERROR_INITIALIZATION_FAILED:
        text = TEXT("VK_ERROR_INITIALIZATION_FAILED");
        break;
    case VK_ERROR_DEVICE_LOST:
        text = TEXT("VK_ERROR_DEVICE_LOST");
        break;
    case VK_ERROR_MEMORY_MAP_FAILED:
        text = TEXT("VK_ERROR_MEMORY_MAP_FAILED");
        break;
    case VK_ERROR_LAYER_NOT_PRESENT:
        text = TEXT("VK_ERROR_LAYER_NOT_PRESENT");
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        text = TEXT("VK_ERROR_EXTENSION_NOT_PRESENT");
        break;
    case VK_ERROR_FEATURE_NOT_PRESENT:
        text = TEXT("VK_ERROR_FEATURE_NOT_PRESENT");
        break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        text = TEXT("VK_ERROR_INCOMPATIBLE_DRIVER");
        break;
    case VK_ERROR_TOO_MANY_OBJECTS:
        text = TEXT("VK_ERROR_TOO_MANY_OBJECTS");
        break;
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        text = TEXT("VK_ERROR_FORMAT_NOT_SUPPORTED");
        break;
    case VK_ERROR_FRAGMENTED_POOL:
        text = TEXT("VK_ERROR_FRAGMENTED_POOL");
        break;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        text = TEXT("VK_ERROR_OUT_OF_POOL_MEMORY");
        break;
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        text = TEXT("VK_ERROR_INVALID_EXTERNAL_HANDLE");
        break;
    case VK_ERROR_SURFACE_LOST_KHR:
        text = TEXT("VK_ERROR_SURFACE_LOST_KHR");
        break;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        text = TEXT("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
        break;
    case VK_SUBOPTIMAL_KHR:
        text = TEXT("VK_SUBOPTIMAL_KHR");
        break;
    case VK_ERROR_OUT_OF_DATE_KHR:
        text = TEXT("VK_ERROR_OUT_OF_DATE_KHR");
        break;
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        text = TEXT("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
        break;
    case VK_ERROR_VALIDATION_FAILED_EXT:
        text = TEXT("VK_ERROR_VALIDATION_FAILED_EXT");
        break;
    case VK_ERROR_INVALID_SHADER_NV:
        text = TEXT("VK_ERROR_INVALID_SHADER_NV");
        break;
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        text = TEXT("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT");
        break;
    case VK_ERROR_FRAGMENTATION_EXT:
        text = TEXT("VK_ERROR_FRAGMENTATION_EXT");
        break;
    case VK_ERROR_NOT_PERMITTED_EXT:
        text = TEXT("VK_ERROR_NOT_PERMITTED_EXT");
        break;
    case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
        text = TEXT("VK_ERROR_INVALID_DEVICE_ADDRESS_EXT");
        break;
    default:
        
        break;
	}
    BearLog::DebugPrintf(TEXT("Vulkan ERROR:%s"), text);
    BEAR_ASSERT(0);
}