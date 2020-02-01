#pragma once
class VKUniformBuffer :public BearRHI::BearRHIUniformBuffer
{
	BEAR_CLASS_WITHOUT_COPY(VKUniformBuffer);
public:
	VKUniformBuffer();
	virtual void Create(bsize Size, bool Dynamic);
	virtual ~VKUniformBuffer();
	virtual void* Lock();
	virtual void Unlock();
	virtual void Clear();

	VkBuffer Buffer;
	bsize Size;
	VkDescriptorBufferInfo BufferInfo;
private:
	VkDeviceMemory Memory;
	bool m_dynamic;

};