#pragma once
class VKRenderIndexBuffer :public BearRenderBase::BearRenderIndexBufferBase
{
public:
	VKRenderIndexBuffer();
	virtual ~VKRenderIndexBuffer();
	virtual void Create(bsize count, void*data = 0);
	virtual int32* Lock();
	virtual void Unlock();
	virtual void Clear();
	VkBuffer Buffer;

private:
	VkMemoryRequirements MRequirements;
	VkDeviceMemory Memory;
};