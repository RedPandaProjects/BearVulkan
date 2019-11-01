#include "vulkanPCH.h"
#include "VKRenderBufferTool.h"
VKRenderUniformBuffer::VKRenderUniformBuffer():m_dynamic(false)
{
	m_buffer = 0;
	Buffer = 0;
	Memory = 0;
	m_size = 0;
}

VKRenderUniformBuffer::~VKRenderUniformBuffer()
{
	Clear();
}
inline bool Test(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {

	for (uint32_t i = 0; i < Factory->PhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((Factory->PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	return false;
}

void VKRenderUniformBuffer::Create(bsize size, void * data, bool dynamic )
{
	Clear();
	m_dynamic = dynamic;
	
	if(dynamic)
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, Memory);
	else
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, Memory);
	BufferInfo.buffer = Buffer;
	BufferInfo.offset = 0;
	BufferInfo.range = static_cast<VkDeviceSize>(size);
	m_size = size;
	if (data)
	{
		bear_copy(Lock(), data, size);
		Unlock();
	}
}

void * VKRenderUniformBuffer::Lock()
{
	if (Memory == 0)return 0;
	if (m_dynamic)
	{
		uint8_t *pData;
		V_CHK(vkMapMemory(Factory->Device, Memory, 0, m_size, 0, (void **)&pData));
		return pData;
	}
	else 
	{
		if (m_buffer)
			bear_free(m_buffer);
		m_buffer = bear_alloc<uint8>(m_size);
		return m_buffer;
	}

}

void VKRenderUniformBuffer::Unlock()
{
	if (Memory == 0)return;
	if (m_dynamic)
	{
		vkUnmapMemory(Factory->Device, Memory);
	}
	else if (m_buffer)
	{
		VkBuffer BufferTemp;
		VkDeviceMemory MemoryTemp;
		CreateBuffer(Factory->PhysicalDevice, Factory->Device, m_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, BufferTemp, MemoryTemp);
		{
			uint8_t *pData;
			V_CHK(vkMapMemory(Factory->Device, MemoryTemp, 0, m_size, 0, (void **)&pData));
			bear_copy(pData, m_buffer, m_size);
			vkUnmapMemory(Factory->Device, MemoryTemp);
		}
		CopyBuffer(BufferTemp, Buffer, m_size);
		vkDestroyBuffer(Factory->Device, BufferTemp, 0);
		vkFreeMemory(Factory->Device, MemoryTemp, 0);
		bear_free(m_buffer);
		m_buffer = 0;
	}
}

void VKRenderUniformBuffer::Clear()
{
	m_size = 0;
	m_dynamic = false;
	if (m_buffer)
		bear_free(m_buffer);
	m_buffer = 0;
	if (Buffer)vkDestroyBuffer(Factory->Device, Buffer,0);
	Buffer = 0;
	if (Memory)vkFreeMemory(Factory->Device, Memory, 0);
	Memory = 0;
}
