#include "VKPCH.h"
size_t IndexBufferCounter = 0;
VKIndexBuffer::VKIndexBuffer()
{
	Buffer = 0;
	Memory = 0;
	m_dynamic = false; Size = 0;
	IndexBufferCounter++;
}

void VKIndexBuffer::Create(size_t Count, bool Dynamic, void* data)
{
	Clear();
	m_dynamic = Dynamic;

	if (Dynamic)
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Count * sizeof(uint32), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, Memory);
	else
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Count * sizeof(uint32), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, Memory);
	Size = Count * sizeof(uint32);
	if (data && !Dynamic)
	{
		VkBuffer TempBuffer;
		VkDeviceMemory TempMemory;
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, TempBuffer, TempMemory);

		uint8_t* pData;
		V_CHK(vkMapMemory(Factory->Device, TempMemory, 0, Size, 0, (void**)&pData));
		memcpy(pData, data, Size);
		vkUnmapMemory(Factory->Device, TempMemory);

		Factory->LockCommandBuffer();
		CopyBuffer(Factory->CommandBuffer, TempBuffer, Buffer, Size);
		Factory->UnlockCommandBuffer();
		vkDestroyBuffer(Factory->Device, TempBuffer, 0);
		vkFreeMemory(Factory->Device, TempMemory, 0);
	}
	else if (data)
	{
		memcpy(Lock(), data, Count * sizeof(uint32));
		Unlock();
	}
}

VKIndexBuffer::~VKIndexBuffer()
{
	IndexBufferCounter--;
	Clear();
}

uint32* VKIndexBuffer::Lock()
{
	if (Memory == 0)return 0;
	BEAR_CHECK(m_dynamic);
	uint8_t* pData;
	V_CHK(vkMapMemory(Factory->Device, Memory, 0, Size, 0, (void**)&pData));
	return (uint32*)pData;

}

void VKIndexBuffer::Unlock()
{
	if (Memory == 0)return;
	vkUnmapMemory(Factory->Device, Memory);
}

void VKIndexBuffer::Clear()
{
	Size = 0;
	m_dynamic = false;
	if (Buffer)vkDestroyBuffer(Factory->Device, Buffer, 0);
	Buffer = 0;
	if (Memory)vkFreeMemory(Factory->Device, Memory, 0);
	Memory = 0;
}

size_t VKIndexBuffer::GetCount()
{
	return Size/sizeof(uint32);
}
