#pragma once
class VKContext :public BearRHI::BearRHIContext
{
public:
	VKContext();
	virtual ~VKContext();

	virtual void BeginEvent(char const* name, BearColor color = BearColor::White);
	virtual void EndEvent();

	virtual void Reset();
	virtual void Wait();
	virtual void Flush(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport, bool wait);
	virtual void Flush(bool wait);
	virtual void ClearState();

	virtual void Copy(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Src);
	virtual void Copy(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Src);
	virtual void Copy(BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Src);
	virtual void Copy(BearFactoryPointer<BearRHI::BearRHITexture2D> Dst, BearFactoryPointer<BearRHI::BearRHITexture2D> Src);

	virtual void SetViewportAsFrameBuffer(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport);
	virtual void SetFrameBuffer(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> FrameBuffer);
	virtual void SetPipeline(BearFactoryPointer<BearRHI::BearRHIPipeline> Pipeline);
	virtual void SetDescriptorHeap(BearFactoryPointer<BearRHI::BearRHIDescriptorHeap> DescriptorHeap);
	virtual void SetVertexBuffer(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> buffer);
	virtual void SetIndexBuffer(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> buffer);
	virtual void SetViewport(float x, float y, float width, float height, float minDepth = 0.f, float maxDepth = 1.f);
	virtual void SetScissor(bool Enable, float x, float y, float x1, float y1);
	virtual void SetStencilRef(uint32 ref);
	virtual void Draw(size_t count, size_t offset = 0);
	virtual void DrawIndex(size_t count, size_t  offset_index = 0, size_t  offset_vertex = 0);
	virtual void DispatchMesh(size_t CountX, size_t CountY, size_t CountZ);

	virtual void Lock(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport);
	virtual void Unlock(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport);
	virtual void Lock(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> FrameBuffer);
	virtual void Unlock(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> FrameBuffer);

private:
	bool m_UseRenderPass;
	VkViewport Viewport;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
	VkSemaphore SemaphoreWait;
	VkFence Fence;

};