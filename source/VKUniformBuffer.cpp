#include "VKPCH.h"
bsize UniformBufferCounter = 0;
VKUniformBuffer::VKUniformBuffer() :m_dynamic(false)
{
	Buffer = 0;
	Memory = 0;
	Size = 0;
	UniformBufferCounter++;
}

void VKUniformBuffer::Create(bsize Size_, bool Dynamic)
{
	Clear();
	m_dynamic = Dynamic;
	
	if (Dynamic)
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Size_, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT| VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, Memory);
	else
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Size_, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, Memory);
	BEAR_ASSERT(Memory != 0);
	Size = Size_;
	BufferInfo.buffer = Buffer;
	BufferInfo.offset = 0;
	BufferInfo.range = Size_;

}

VKUniformBuffer::~VKUniformBuffer()
{
	UniformBufferCounter--;
	Clear();
}

void* VKUniformBuffer::Lock()
{
	BEAR_ASSERT(Memory != 0);
	if (Memory == 0)return 0;
	BEAR_ASSERT(m_dynamic);
	uint8_t* pData;
	V_CHK(vkMapMemory(Factory->Device, Memory, 0, Size, 0, (void**)&pData));
	BEAR_ASSERT(pData);
	return (uint32*)pData;
}

void VKUniformBuffer::Unlock()
{
	if (Memory == 0)return;
	vkUnmapMemory(Factory->Device, Memory);
}

void VKUniformBuffer::Clear()
{
	Size = 0;
	m_dynamic = false;
	if (Buffer)vkDestroyBuffer(Factory->Device, Buffer, 0);
	Buffer = 0;
	if (Memory)vkFreeMemory(Factory->Device, Memory, 0);
	Memory = 0;
}

