#pragma once 
class VKFrameBuffer :public BearRHI::BearRHIFrameBuffer
{
public:
	VKFrameBuffer(const BearFrameBufferDescription& Description);
	virtual ~VKFrameBuffer();
	VkRenderPassBeginInfo GetRenderPass();
	void Unlock(VkCommandBuffer Cmd);
	void Lock(VkCommandBuffer Cmd);
	BearFrameBufferDescription Description;
	size_t CountRenderTarget;
	VkFramebuffer FrameBuffer;
	VKRenderPass *RenderPassRef;
	VkClearValue ClearValues[9];
	uint32_t Width;
	uint32_t Height;
};