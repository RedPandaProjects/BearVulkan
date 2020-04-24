#pragma once
class VKVertexBuffer :public BearRHI::BearRHIVertexBuffer
{
	//BEAR_CLASS_WITHOUT_COPY(VKVertexBuffer);
public:
	VKVertexBuffer();
	virtual void Create(size_t Stride,size_t Count, bool Dynamic,void *data);
	virtual ~VKVertexBuffer();
	virtual void* Lock();
	virtual void Unlock();
	virtual void Clear();
	virtual size_t GetCount();
	VkBuffer Buffer;
	VkVertexInputBindingDescription VertexDescription;
	size_t Size;
private:
	VkDeviceMemory Memory;
	bool m_dynamic;

};