#pragma once
class VKPipeline :public BearRHI::BearRHIPipeline
{
	BEAR_CLASS_WITHOUT_COPY(VKPipeline);
public:
	VKPipeline(const BearPipelineDescription&desc);
	virtual ~VKPipeline();
	VkPipeline Pipeline;

};