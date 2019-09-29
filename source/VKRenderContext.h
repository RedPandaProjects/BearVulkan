#pragma once
class VKRenderContext :public BearRenderBase::BearRenderContextBase
{
public:
	BEAR_CLASS_NO_COPY(VKRenderContext);
	VKRenderContext();
	virtual ~VKRenderContext();

	virtual void Flush();
	virtual void AttachViewportAsFrameBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> Viewport);
	virtual void DetachFrameBuffer();
	virtual void ClearFrameBuffer();

private:
	BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> m_viewport;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
};