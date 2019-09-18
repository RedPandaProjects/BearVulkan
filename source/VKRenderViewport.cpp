#include "vulkanPCH.h"

VKRenderViewport::VKRenderViewport(void * Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync)
{
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = NULL;
		createInfo.hinstance = GetModuleHandle(0);
		createInfo.hwnd = (HWND)Handle;
		V_CHK(vkCreateWin32SurfaceKHR(Factory->Instance, &createInfo, NULL, &Surface));
	}

	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, Width, Height);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	V_CHK (vkCreateSwapchainKHR(Factory->Device, &createInfo, nullptr, &SwapChain)) 

	V_CHK(vkGetSwapchainImagesKHR(Factory->Device, SwapChain, &imageCount, nullptr));
	SwapChainImages.resize(imageCount);
	V_CHK(vkGetSwapchainImagesKHR(Factory->Device, SwapChain, &imageCount, SwapChainImages.data()));

	SwapChainImageFormat = surfaceFormat.format;
	SwapChainExtent = extent;

	SwapChainImageViews.resize(SwapChainImages.size());
	for (uint32_t i = 0; i < SwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo color_image_view = {};
		color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		color_image_view.pNext = NULL;
		color_image_view.flags = 0;
		color_image_view.image = SwapChainImages[i];
		color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		color_image_view.format = SwapChainImageFormat;
		color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
		color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
		color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
		color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
		color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		color_image_view.subresourceRange.baseMipLevel = 0;
		color_image_view.subresourceRange.levelCount = 1;
		color_image_view.subresourceRange.baseArrayLayer = 0;
		color_image_view.subresourceRange.layerCount = 1;

		V_CHK(vkCreateImageView(Factory->Device, &color_image_view, NULL, &SwapChainImageViews[i]));
	}
	{
		VkAttachmentDescription attachments[2];
		attachments[0].format = SwapChainImageFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
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

		V_CHK( vkCreateRenderPass(Factory->Device, &rp_info, NULL, &RenderPass));
	}
	{
		VkImageView attachments[1];

		VkFramebufferCreateInfo fb_info = {};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.pNext = NULL;
		fb_info.renderPass = RenderPass;
		fb_info.attachmentCount = 1;
		fb_info.pAttachments = attachments;
		fb_info.width =static_cast<uint32_t>( Width);
		fb_info.height = static_cast<uint32_t>(Height); ;
		fb_info.layers = 1;


		Framebuffers.resize(SwapChainImages.size());

		for (uint32_t i = 0; i < Framebuffers.size(); i++)
		{
			attachments[0] = SwapChainImageViews[i];
			V_CHK(vkCreateFramebuffer(Factory->Device, &fb_info, NULL, &Framebuffers[i]));
		}
	}
}

VKRenderViewport::~VKRenderViewport()
{
	for (uint32_t i = 0; i < Framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(Factory->Device, Framebuffers[i],0);
	}
	for (uint32_t i = 0; i < SwapChainImageViews.size(); i++)
	{
		vkDestroyImageView(Factory->Device, SwapChainImageViews[i], NULL);
	}
	
	vkDestroySwapchainKHR(Factory->Device, SwapChain, NULL);
	vkDestroySurfaceKHR(Factory->Instance, Surface, 0);
}

void VKRenderViewport::SetVSync(bool Sync)
{
}

void VKRenderViewport::SetFullScreen(bool FullScreen)
{
}

void VKRenderViewport::Resize(bsize Width, bsize Height)
{
}

void * VKRenderViewport::GetHandle()
{
	return this;
}

void VKRenderViewport::Swap()
{
}

VKRenderViewport::SwapChainSupportDetails VKRenderViewport::QuerySwapChainSupport()
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Factory->PhysicalDevice, Surface, &details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Factory->PhysicalDevice, Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Factory->PhysicalDevice, Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Factory->PhysicalDevice, Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Factory->PhysicalDevice, Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR VKRenderViewport::ChooseSwapSurfaceFormat(const BearVector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VKRenderViewport::ChooseSwapPresentMode(const BearVector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VKRenderViewport::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, bsize Width, bsize Height)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = {static_cast<uint32_t>( Width),static_cast<uint32_t>(Height) };

		actualExtent.width = bear_max(capabilities.minImageExtent.width, bear_min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = bear_max(capabilities.minImageExtent.height, bear_min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VKRenderViewport::QueueFamilyIndices VKRenderViewport::FindQueueFamilies()
{
	QueueFamilyIndices indices;
	bool graphicsFamilyEmpty = false;
	bool presentFamilyEmpty = false;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Factory->PhysicalDevice, &queueFamilyCount, nullptr);

	BearVector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(Factory->PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamilyEmpty = true;
			indices.graphicsFamily = i;

		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Factory->PhysicalDevice, i, Surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			presentFamilyEmpty = true;
			indices.presentFamily = i;
		}

		if (presentFamilyEmpty&&graphicsFamilyEmpty)
		{
			break;
		}

		i++;
	}

	return indices;
}
