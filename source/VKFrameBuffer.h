#pragma once 
class VKFrameBuffer :public BearRHI::BearRHIFrameBuffer
{
public:
	VKFrameBuffer(const BearFrameBufferDescription& Description);
	virtual ~VKFrameBuffer();
	VkRenderPassBeginInfo GetRenderPass();
	void ToTexture(VkCommandBuffer Cmd);
	void ToRT(VkCommandBuffer Cmd);
	BearFrameBufferDescription Description;
	bsize CountRenderTarget;
	VkFramebuffer FrameBuffer;
	VKRenderPass *RenderPassRef;
	VkClearValue ClearValues[9];
	uint32_t Width;
	uint32_t Height;
};