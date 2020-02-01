#pragma once
class VKShaderResource :public virtual BearRHI::BearRHIShaderResource
{
public:
	virtual void Set(VkWriteDescriptorSet* HEAP) = 0;
};