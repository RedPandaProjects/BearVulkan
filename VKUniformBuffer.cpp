#include "VKPCH.h"
size_t UniformBufferCounter = 0;

VKUniformBuffer::VKUniformBuffer(size_t stride, size_t count, bool Dynamic)
{
	m_dynamic = Dynamic;

	Stride = (static_cast<size_t>((stride + Factory->PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment - 1) & ~(Factory->PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment - 1)));
	Count = count;
	if (Dynamic)
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Stride* Count, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, Memory);
	else
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, Stride* Count, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, Memory);
	BEAR_CHECK(Memory != 0);

}



VKUniformBuffer::~VKUniformBuffer()
{
	if (Buffer)vkDestroyBuffer(Factory->Device, Buffer, 0);
	if (Memory)vkFreeMemory(Factory->Device, Memory, 0);
	UniformBufferCounter--;
}

void* VKUniformBuffer::Lock()
{
	BEAR_CHECK(Memory != 0);
	if (Memory == 0)return 0;
	BEAR_CHECK(m_dynamic);
	uint8_t* pData;
	V_CHK(vkMapMemory(Factory->Device, Memory, 0, Stride*Count, 0, (void**)&pData));
	BEAR_CHECK(pData);
	return (uint32*)pData;
}

void VKUniformBuffer::Unlock()
{
	if (Memory == 0)return;
	vkUnmapMemory(Factory->Device, Memory);
}

size_t VKUniformBuffer::GetStride()
{
	return Stride;
}

size_t VKUniformBuffer::GetCount()
{
	return Count;
}

