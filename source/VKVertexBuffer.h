#pragma once
class VKVertexBuffer :public BearRHI::BearRHIVertexBuffer
{
	BEAR_CLASS_WITHOUT_COPY(VKVertexBuffer);
public:
	VKVertexBuffer();
	virtual void Create(bsize Stride,bsize Count, bool Dynamic);
	virtual ~VKVertexBuffer();
	virtual void* Lock();
	virtual void Unlock();
	virtual void Clear();
	virtual bsize GetCount();
	VkBuffer Buffer;
	VkVertexInputBindingDescription VertexDescription;
	bsize Size;
private:
	VkDeviceMemory Memory;
	bool m_dynamic;

};