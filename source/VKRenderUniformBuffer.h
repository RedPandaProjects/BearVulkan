#pragma once
class VKRenderUniformBuffer :public BearRenderBase::BearRenderUniformBufferBase
{
public:
	VKRenderUniformBuffer();
	virtual ~VKRenderUniformBuffer();
	virtual void Create(bsize size, void*data = 0, bool dynamic=false);
	virtual void* Lock();
	virtual void Unlock();
	virtual void Clear();
	VkBuffer Buffer;

private:
	VkDeviceMemory Memory;
	bool m_dynamic;
	uint8*m_buffer;
	bsize m_size;
};