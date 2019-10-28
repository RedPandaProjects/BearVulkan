#pragma once
class VKRenderVertexBuffer :public BearRenderBase::BearRenderVertexBufferBase
{
public:
	VKRenderVertexBuffer();
	virtual ~VKRenderVertexBuffer();
	virtual void Create(bsize stride, bsize count, void*data = 0, bool dynamic=false);
	virtual void* Lock();
	virtual void Unlock();
	virtual void Clear();
	VkBuffer Buffer;
	VkVertexInputBindingDescription VertexDescription;

private:
	VkMemoryRequirements MRequirements;
	VkDeviceMemory Memory;
};