#pragma once
class VKRenderViewport :public BearRenderBase::BearRenderViewportBase
{
public:
	BEAR_CLASS_NO_COPY(VKRenderViewport);
	VKRenderViewport(void * Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync, const BearGraphics::BearRenderViewportDescription&Description);
	virtual ~VKRenderViewport();
	virtual void SetVSync(bool Sync);
	virtual void SetFullScreen(bool FullScreen);
	virtual void Resize(bsize Width, bsize Height);
	virtual void*GetHandle();
	void Swap(const VkCommandBuffer &cmd);
	VkSemaphore Semaphore;
	VkSwapchainKHR SwapChain;
	BearVector<VkImage> SwapChainImages;
	BearVector<VkImageView> SwapChainImageViews;
	BearVector<VkFramebuffer> Framebuffers;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	VkRenderPass RenderPass;
	BearCore::BearColor ClearColor;
	uint32_t FrameIndex;
	bsize Width;
	bsize Height;

	VkQueue PresentQueue;
	VkRenderPassBeginInfo GetRenderPass();
	VkFence PresentFence;
	VkFence Fence;
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


	uint32_t FindQueueFamilies();
};