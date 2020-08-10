#pragma once
class VKUnorderedAccess :public virtual  BearRHI::BearRHIUnorderedAccess, public VKShaderResource
{
public:
	VKUnorderedAccess() { bAllowUAV = false; }
	virtual void SetAsUAV(VkWriteDescriptorSet* HEAP, size_t offset) = 0;
	virtual void LockUAV(VkCommandBuffer  CommandLine) = 0;
	virtual void UnlockUAV(VkCommandBuffer  CommandLine) = 0;
protected:
	bool bAllowUAV;
};