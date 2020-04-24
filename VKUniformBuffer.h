#pragma once
class VKUniformBuffer :public BearRHI::BearRHIUniformBuffer
{
	//BEAR_CLASS_WITHOUT_COPY(VKUniformBuffer);
public:
	VKUniformBuffer(size_t Stride, size_t Count, bool Dynamic);
	virtual ~VKUniformBuffer();
	virtual void* Lock();
	virtual void Unlock();
	virtual size_t GetStride();
	virtual size_t GetCount();
	VkBuffer Buffer;
	size_t Stride;
	size_t Count;
private:
	VkDeviceMemory Memory;
	bool m_dynamic;

};