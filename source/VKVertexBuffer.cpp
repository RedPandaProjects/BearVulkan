#include "VKPCH.h"

VKVertexBuffer::VKVertexBuffer() :m_dynamic(false)
{
	Buffer = 0;
	Memory = 0;
	Size = 0;
}

void VKVertexBuffer::Create(bsize Stride, bsize Count, bool Dynamic)
{
	Clear();
	m_dynamic = Dynamic;

	if (Dynamic)
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Count * Stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT| VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, Memory);
	else
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Count * Stride, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, Memory);
	VertexDescription.binding = 0;
	VertexDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VertexDescription.stride = static_cast<uint32_t>(Stride);
	Size = Stride * Count;

}

VKVertexBuffer::~VKVertexBuffer()
{
	Clear();
}

void* VKVertexBuffer::Lock()
{
	if (Memory == 0)return 0;
	BEAR_ASSERT(m_dynamic);
	uint8_t* pData;
	V_CHK(vkMapMemory(Factory->Device, Memory, 0, Size, 0, (void**)&pData));
	return (uint32*)pData;
}

void VKVertexBuffer::Unlock()
{
	if (Memory == 0)return;
	vkUnmapMemory(Factory->Device, Memory);
}

void VKVertexBuffer::Clear()
{
	Size = 0;
	m_dynamic = false;
	if (Buffer)vkDestroyBuffer(Factory->Device, Buffer, 0);
	Buffer = 0;
	if (Memory)vkFreeMemory(Factory->Device, Memory, 0);
	Memory = 0;
}
