#pragma once
class VKViewport :public BearRHI::BearRHIViewport
{
public:
	BEAR_CLASS_WITHOUT_COPY(VKViewport);
	VKViewport(void * Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync, const BearViewportDescription&Description);
	virtual ~VKViewport();
	virtual void SetVSync(bool Sync);
	virtual void SetFullScreen(bool FullScreen);
	virtual void Resize(bsize Width, bsize Height);
	virtual void Copy(BearFactoryPointer<BearRHI::BearRHITexture2D> Dst);
	void Swap();
	virtual BearRenderTargetFormat GetFormat();
	VkSemaphore Semaphore;

	VkSwapchainKHR SwapChain;
	BearVector<VkImage> SwapChainImages;
	BearVector<VkImageView> SwapChainImageViews;
	BearVector<VkFramebuffer> Framebuffers;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	VkRenderPass RenderPass;
	BearColor ClearColor;
	uint32_t FrameIndex;
	bsize Width;
	bsize Height;
	bool VSync;

	VkQueue PresentQueue;
	VkRenderPassBeginInfo GetRenderPass();
	VkFence PresentFence;
private:
	BearViewportDescription Description;
	uint32_t m_PresentQueueFamilyIndex;
	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR capabilities;
		BearVector<VkSurfaceFormatKHR> formats;
		BearVector<VkPresentModeKHR> presentModes;
	};
	VkClearValue m_ClearValues[1];
	SwapChainSupportDetails QuerySwapChainSupport();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const BearVector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const BearVector<VkPresentModeKHR>& availablePresentModes,bool vsync);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, bsize Width, bsize Height);
	VkSurfaceKHR Surface;


	uint32_t FindQueueFamilies();

	void CreateSwapChain(bsize Width, bsize Height,bool vsync);
	void CreateImageView();
	void DestroyImageView();
	void DestroySwapChain(VkSwapchainKHR swapChain);
	void CreateFrameBuffers();
	void DestroyFrameBuffers();
	bool m_FullScreen;
	HWND m_WindowHandle;

};