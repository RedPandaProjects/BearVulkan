#pragma once
class VKContext :public BearRHI::BearRHIContext
{
public:
	BEAR_CLASS_WITHOUT_COPY(VKContext);
	VKContext();
	virtual ~VKContext();
	virtual void Wait();
	virtual void Flush(bool wiat);
	virtual void AttachViewportAsFrameBuffer(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport);


	virtual void DetachFrameBuffer();
	virtual void ClearFrameBuffer();



private:
	void PreDestroy();
	int UpdateStatus();
	int8 m_Status;

	BearFactoryPointer<BearRHI::BearRHIViewport> m_viewport;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
	VkSemaphore SemaphoreWait;
	VkFence Fence;

	VkViewport Viewport;
	VkRect2D Scissor;

	bool ScissorEnable;
};