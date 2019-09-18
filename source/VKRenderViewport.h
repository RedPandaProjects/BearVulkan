#pragma once
class VKRenderViewport :public BearRenderBase::BearRenderViewportBase
{
public:
	BEAR_CLASS_NO_COPY(VKRenderViewport);
	VKRenderViewport(void * Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync);
	virtual ~VKRenderViewport();
	virtual void SetVSync(bool Sync);
	virtual void SetFullScreen(bool FullScreen);
	virtual void Resize(bsize Width, bsize Height);
	virtual void*GetHandle();
	void Swap();
	VkSwapchainKHR SwapChain;
	BearVector<VkImage> SwapChainImages;
	BearVector<VkImageView> SwapChainImageViews;
	BearVector<VkFramebuffer> Framebuffers;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	VkRenderPass RenderPass;
private:

	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR capabilities;
		BearVector<VkSurfaceFormatKHR> formats;
		BearVector<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails QuerySwapChainSupport();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const BearVector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const BearVector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, bsize Width, bsize Height);
	VkSurfaceKHR Surface;

	struct QueueFamilyIndices 
	{
		QueueFamilyIndices():graphicsFamily(0), presentFamily(0){}
		uint32_t graphicsFamily;
	
		uint32_t presentFamily;

	};
	QueueFamilyIndices FindQueueFamilies();
};