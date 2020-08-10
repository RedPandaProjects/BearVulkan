#pragma once
class VKStructuredBuffer : public VKUnorderedAccess, public BearRHI::BearRHIStructuredBuffer
{
public:
	VKStructuredBuffer(size_t Size, void* Data,bool UAV);
	virtual ~VKStructuredBuffer();
	virtual void SetAsSRV(VkWriteDescriptorSet* HEAP, size_t offset);
	virtual void SetAsUAV(VkWriteDescriptorSet* HEAP, size_t offset);
	virtual void LockUAV(VkCommandBuffer  CommandLine);
	virtual void UnlockUAV(VkCommandBuffer  CommandLine);
	virtual void* QueryInterface(int Type);
	VkBuffer Buffer;
	VkDescriptorBufferInfo BufferInfo;
	VkDeviceMemory Memory;
	//VkBufferView BufferView;
	size_t Size;
};

