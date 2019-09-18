#pragma once
class VKRenderContext :public BearRenderBase::BearRenderContextBase
{
public:
	BEAR_CLASS_NO_COPY(VKRenderContext);
	VKRenderContext();
	virtual ~VKRenderContext();

	virtual void Flush();
	virtual void AttachViewport(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> Viewport);
	virtual void DetachViewport();
	virtual void ClearColor(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderTargetViewBase> RenderTarget, const BearCore::BearColor Color);

private:
	BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> m_viewport;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
};