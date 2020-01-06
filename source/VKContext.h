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
	virtual void Copy(BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Src);

	virtual void SetDescriptorHeap(BearFactoryPointer<BearRHI::BearRHIDescriptorHeap> DescriptorHeap);
	virtual void SetPipeline(BearFactoryPointer<BearRHI::BearRHIPipeline> Pipeline);
	virtual void SetVertexBuffer(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> buffer);
	virtual void SetIndexBuffer(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> buffer);
	virtual void SetViewport(float x, float y, float width, float height, float minDepth = 0.f, float maxDepth = 1.f);
	virtual void SetScissor(bool Enable, float x, float y, float x1, float y1);
	virtual void Draw(bsize count, bsize offset = 0);
	virtual void DrawIndex(bsize count, bsize  offset_index, bsize offset_vertex);

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
private:
	bool m_use_renderpass;
};