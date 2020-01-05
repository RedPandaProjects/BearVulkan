#pragma once
class VKIndexBuffer :public BearRHI::BearRHIIndexBuffer
{
	BEAR_CLASS_WITHOUT_COPY(VKIndexBuffer);
public:
	VKIndexBuffer();
	virtual void Create(bsize Count, bool Dynamic);
	virtual ~VKIndexBuffer();
	virtual uint32* Lock();
	virtual void Unlock();
	virtual void Clear();
	VkBuffer Buffer;
	bsize Size;
private:
	VkDeviceMemory Memory;
	bool m_dynamic;

};