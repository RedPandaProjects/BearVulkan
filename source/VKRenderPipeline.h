#pragma once
class VKRenderPipeline :public BearRenderBase::BearRenderPipelineBase
{
	BEAR_CLASS_NO_COPY(VKRenderPipeline);
public:
	VKRenderPipeline(const BearGraphics::BearRenderPipelineDescription&desc);
	virtual ~VKRenderPipeline();
	VkPipeline Pipeline;
};