#include "VKPCH.h"
bsize RayTracingShaderTableCounter = 0;
#ifdef RTX
VKRayTracingShaderTable::VKRayTracingShaderTable(const BearRayTracingShaderTableDescription& Description)
{
	RayTracingShaderTableCounter++;
	VKPipelineRayTracing* Pipeline = reinterpret_cast<VKPipelineRayTracing*>(const_cast<BearRHI::BearRHIPipelineRayTracing*>(Description.Pipeline.get())->QueryInterface(VKQ_RayTracingPipeline));
	BEAR_CHECK(Pipeline);
	void*Data = Pipeline->GetShaderIdentifier(*Description.Name);
    Size = Factory->PhysicalDeviceRayTracingProperties.shaderGroupHandleSize;
    Size = (Size + (Factory->PhysicalDeviceRayTracingProperties.shaderGroupBaseAlignment - 1)) & Factory->PhysicalDeviceRayTracingProperties.shaderGroupBaseAlignment;
	{
        CreateBuffer(Factory->PhysicalDevice, Factory->Device, Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, Memory);
        uint8_t* pData;
        V_CHK(vkMapMemory(Factory->Device, Memory, 0, Size, 0, (void**)&pData));
        memcpy(pData, Data, Size);
        vkUnmapMemory(Factory->Device, Memory);
	}
}

VKRayTracingShaderTable::~VKRayTracingShaderTable()
{
	RayTracingShaderTableCounter--;
    vkDestroyBuffer(Factory->Device, Buffer, 0);
    vkFreeMemory(Factory->Device, Memory, 0);
}
#endif