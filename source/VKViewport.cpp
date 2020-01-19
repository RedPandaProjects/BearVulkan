#include "VKPCH.h"

VKViewport::VKViewport(void * Handle, bsize Width_, bsize Height_, bool Fullscreen, bool VSync, const BearViewportDescription&Description_):Width(Width_),Height(Height_), Description(Description_)
{
	
	m_FullScreen = 0;
	m_WindowHandle = (HWND)Handle;
	ClearColor = Description.ClearColor;
	RenderPass = VK_NULL_HANDLE;
	SwapChain = VK_NULL_HANDLE;
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = NULL;
		createInfo.hinstance = GetModuleHandle(0);
		createInfo.hwnd = (HWND)Handle;
		V_CHK(vkCreateWin32SurfaceKHR(Factory->Instance, &createInfo, NULL, &Surface));
	}

	m_PresentQueueFamilyIndex = FindQueueFamilies();


	


	CreateSwapChain(Width,Height, VSync);
	{
		VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
		imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		imageAcquiredSemaphoreCreateInfo.pNext = NULL;
		imageAcquiredSemaphoreCreateInfo.flags = 0;

		V_CHK( vkCreateSemaphore(Factory->Device, &imageAcquiredSemaphoreCreateInfo, NULL, &Semaphore));

		// Get the index of the next available swapchain image:
		V_CHK(vkAcquireNextImageKHR(Factory->Device,SwapChain, UINT64_MAX, Semaphore, VK_NULL_HANDLE,
			&FrameIndex));
	}
	{

		if (m_PresentQueueFamilyIndex == Factory->QueueFamilyIndex)
		{
			PresentQueue = Factory->Queue;
		}
		else {
			vkGetDeviceQueue(Factory->Device, m_PresentQueueFamilyIndex, 0, &PresentQueue);
		}
		
	}
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		vkCreateFence(Factory->Device, &info, nullptr, &PresentFence);
	}
	SetFullScreen(Fullscreen);
}

VKViewport::~VKViewport()
{
	/*if (PresentQueue != GraphicsQueue)
	{
		vkDestroyQueue
	}*/
	
	vkDestroyFence(Factory->Device, PresentFence,0);
	vkDestroySemaphore(Factory->Device, Semaphore, 0);
	DestroySwapChain(SwapChain);
	vkDestroySurfaceKHR(Factory->Instance, Surface, 0);
}

void VKViewport::SetVSync(bool Sync)
{
	if (VSync != Sync)CreateSwapChain(Width, Height, Sync);
	VSync = Sync;
}

void VKViewport::SetFullScreen(bool FullScreen)
{
	if (m_FullScreen == FullScreen)return;
	if (FullScreen)
	{
		/*HMONITOR hMonitor = MonitorFromWindow((HWND)m_WindowHandle, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFOEX MonitorInfo;
		memset(&MonitorInfo, 0, sizeof(MONITORINFOEX));
		MonitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &MonitorInfo);

		DEVMODE Mode;
		Mode.dmSize = sizeof(DEVMODE);
		Mode.dmBitsPerPel = 32;
		Mode.dmPelsWidth = static_cast<DWORD>(Width);
		Mode.dmPelsHeight = static_cast<DWORD>(Height);
		Mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettingsEx(MonitorInfo.szDevice, &Mode, NULL, CDS_FULLSCREEN, NULL);*/
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<DWORD>(Width);
		dmScreenSettings.dmPelsHeight = static_cast<DWORD>(Height);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);


	}
	else
	{
		ChangeDisplaySettings(NULL, 0);
	}
	m_FullScreen = FullScreen;
}

void VKViewport::Resize(bsize width, bsize height)
{
	if (width != Width || Height != height)CreateSwapChain(Width, Height, VSync);
	Width = width;
	Height = height;
}



void VKViewport::Swap()
{

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.swapchainCount = 1;
	presentInfo.pWaitSemaphores = &Semaphore;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pSwapchains = &SwapChain;
	presentInfo.pImageIndices = &FrameIndex;
	auto result = vkQueuePresentKHR(PresentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		CreateSwapChain(Width, Height, VSync);
	}
	else
	{
		V_CHK(result);
	}
	V_CHK(vkQueueSubmit(PresentQueue, 0, nullptr, PresentFence));
	V_CHK(vkWaitForFences(Factory->Device, 1, &PresentFence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &PresentFence));
	V_CHK(vkAcquireNextImageKHR(Factory->Device, SwapChain, UINT64_MAX, Semaphore, VK_NULL_HANDLE,
		&FrameIndex));

}

VkRenderPassBeginInfo VKViewport::GetRenderPass()
{
	VkRenderPassBeginInfo rp_begin = {};
	rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_begin.pNext = NULL;
	rp_begin.renderPass = RenderPass;
	rp_begin.framebuffer = Framebuffers[FrameIndex];
	rp_begin.renderArea.offset.x = 0;
	rp_begin.renderArea.offset.y = 0;
	rp_begin.renderArea.extent.width = static_cast<uint32_t>( Width);
	rp_begin.renderArea.extent.height = static_cast<uint32_t>(Height);
	rp_begin.clearValueCount = 1;
	m_ClearValues[0].color.float32[0] = ClearColor.R32F;
	m_ClearValues[0].color.float32[1] = ClearColor.G32F;
	m_ClearValues[0].color.float32[2] = ClearColor.B32F;
	m_ClearValues[0].color.float32[3] = ClearColor.A32F;
	rp_begin.pClearValues = m_ClearValues;
	return rp_begin;
}

VKViewport::SwapChainSupportDetails VKViewport::QuerySwapChainSupport()
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

VkSurfaceFormatKHR VKViewport::ChooseSwapSurfaceFormat(const BearVector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VKViewport::ChooseSwapPresentMode(const BearVector<VkPresentModeKHR>& availablePresentModes, bool vsync)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR&& vsync) {
			return availablePresentMode;
		}
		if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR && !vsync) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VKViewport::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, bsize width, bsize height)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = {static_cast<uint32_t>( width),static_cast<uint32_t>(height) };

		actualExtent.width = BearMath::max(capabilities.minImageExtent.width, BearMath::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = BearMath::max(capabilities.minImageExtent.height,BearMath::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

uint32_t VKViewport::FindQueueFamilies()
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Factory->PhysicalDevice, &queueFamilyCount, nullptr);

	BearVector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(Factory->PhysicalDevice, &queueFamilyCount, queueFamilies.data());
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Factory->PhysicalDevice, Factory->QueueFamilyIndex, Surface, &presentSupport);

		if (queueFamilies[Factory->QueueFamilyIndex].queueCount > 0 && presentSupport)
		{
			return Factory->QueueFamilyIndex;
		}
	}
	int i = 0;
	int64 result = -1;
	for (const auto& queueFamily : queueFamilies)
	{

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Factory->PhysicalDevice, i, Surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			result = i; break;
		}


	}

	BEAR_ERRORMESSAGE(result >= 0, TEXT("��������� ���������������� vulkan ��� ������� ����!!!"));
	return static_cast<uint32_t>( result);
}

void VKViewport::CreateSwapChain(bsize width, bsize height, bool vsync)
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes, vsync);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

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


	uint32_t queueFamilyIndices[] = { Factory->QueueFamilyIndex, m_PresentQueueFamilyIndex };

	if (Factory->QueueFamilyIndex != m_PresentQueueFamilyIndex)
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

	createInfo.oldSwapchain = SwapChain;

	V_CHK(vkCreateSwapchainKHR(Factory->Device, &createInfo, nullptr, &SwapChain));
	DestroySwapChain(createInfo.oldSwapchain);
	V_CHK(vkGetSwapchainImagesKHR(Factory->Device, SwapChain, &imageCount, nullptr));
	SwapChainImages.resize(imageCount);
	V_CHK(vkGetSwapchainImagesKHR(Factory->Device, SwapChain, &imageCount, SwapChainImages.data()));
	SwapChainImageFormat = surfaceFormat.format;
	SwapChainExtent = extent;
	CreateImageView();

}

void VKViewport::CreateImageView()
{
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
	CreateFrameBuffers();
}

void VKViewport::DestroyImageView()
{
	for (uint32_t i = 0; i < SwapChainImageViews.size(); i++)
	{
		vkDestroyImageView(Factory->Device, SwapChainImageViews[i], NULL);
	}
}

void VKViewport::DestroySwapChain(VkSwapchainKHR swapChain)
{
	if (swapChain == VK_NULL_HANDLE) return;
	vkDeviceWaitIdle(Factory->Device);
	DestroyFrameBuffers();
	DestroyImageView();
	vkDestroySwapchainKHR(Factory->Device, swapChain, 0);
}

void VKViewport::CreateFrameBuffers()
{

	if(RenderPass==VK_NULL_HANDLE)

	{
		VkAttachmentDescription attachments[2];
		attachments[0].format = SwapChainImageFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = Description.Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
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

		V_CHK(vkCreateRenderPass(Factory->Device, &rp_info, NULL, &RenderPass));
	}
	VkImageView attachments[1];

	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = NULL;
	fb_info.renderPass = RenderPass;
	fb_info.attachmentCount = 1;
	fb_info.pAttachments = attachments;
	fb_info.width = static_cast<uint32_t>(Width);
	fb_info.height = static_cast<uint32_t>(Height); ;
	fb_info.layers = 1;


	Framebuffers.resize(SwapChainImages.size());

	for (uint32_t i = 0; i < Framebuffers.size(); i++)
	{
		attachments[0] = SwapChainImageViews[i];
		V_CHK(vkCreateFramebuffer(Factory->Device, &fb_info, NULL, &Framebuffers[i]));
	}

}

void VKViewport::DestroyFrameBuffers()
{
	for (uint32_t i = 0; i < Framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(Factory->Device, Framebuffers[i], 0);
	}

}

