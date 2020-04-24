#pragma once
class VKStructuredBuffer : public VKShaderResource, public BearRHI::BearRHIStructuredBuffer
{
public:
	VKStructuredBuffer(size_t Size, void* Data);
	virtual ~VKStructuredBuffer();
	virtual void SetAsSRV(VkWriteDescriptorSet* HEAP, size_t offset);
	virtual void* QueryInterface(int Type);
	VkBuffer Buffer;
	VkDescriptorBufferInfo BufferInfo;
	VkDeviceMemory Memory;
	//VkBufferView BufferView;
	size_t Size;
};

