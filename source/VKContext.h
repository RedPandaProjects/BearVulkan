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

	virtual void Copy(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Src);
	virtual void Copy(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Src);

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