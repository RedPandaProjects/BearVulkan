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
	virtual void AttachFrameBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderFrameBufferBase> Framebuffer);

	virtual void DetachFrameBuffer();
	virtual void ClearFrameBuffer();

	virtual void SetPipeline(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderPipelineBase> Pipeline);
	virtual void SetVertexBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderVertexBufferBase> buffer);
	virtual void SetIndexBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderIndexBufferBase> buffer);
	virtual void SetViewport(float x, float y, float width, float height, float minDepth = 0.f, float maxDepth = 1.f);
	virtual void SetScissor(bool Enable, float x, float y, float x1, float y1);
	virtual void Draw(bsize count, bsize offset = 0);
	virtual void DrawIndex(bsize count, bsize offset = 0);
	virtual void  SetDescriptorHeap(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderDescriptorHeapBase> DescriptorHeap);

	virtual void DispatchRays(const BearGraphics::BearRenderDispatchRaysDescription&Description);
private:
	void PreDestroy();
	int UpdateStatus();
	int8 m_Status;

	BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> m_viewport;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
	VkSemaphore SemaphoreWait;
	VkFence Fence;

	VkViewport Viewport;
	VkRect2D Scissor;

	bool ScissorEnable;
};