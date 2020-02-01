#pragma once 
class VKRenderPass :public BearRHI::BearRHIRenderPass
{
public:
	VKRenderPass(const BearRenderPassDescription& Description);
	virtual ~VKRenderPass();
	BearRenderPassDescription Description;
	bsize CountRenderTarget;
	VkRenderPass RenderPass;
};