#pragma once
class VKPipeline :public virtual BearRHI::BearRHIPipeline
{
public:
	virtual void Set(VkCommandBuffer CommandBuffer) = 0;
};
