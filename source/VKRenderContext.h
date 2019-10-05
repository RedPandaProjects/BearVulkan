#pragma once
class VKRenderContext :public BearRenderBase::BearRenderContextBase
{
public:
	BEAR_CLASS_NO_COPY(VKRenderContext);
	VKRenderContext();
	virtual ~VKRenderContext();
	virtual void Wait();
	virtual void Flush(bool wiat);
	virtual void AttachViewportAsFrameBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> Viewport);
	virtual void DetachFrameBuffer();
	virtual void ClearFrameBuffer();

private:
	void PreDestroy();
	int UpdateStatus();
	int8 m_Status;
	VkFence Fence;
	BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> m_viewport;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
	VkSemaphore SemaphoreWait;
};