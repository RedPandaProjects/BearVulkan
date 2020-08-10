#pragma once
class VKRayTracingShaderTable :public BearRHI::BearRHIRayTracingShaderTable
{
	//BEAR_CLASS_WITHOUT_COPY(VKUniformBuffer);
public:
	VKRayTracingShaderTable(const BearRayTracingShaderTableDescription& Description);
	virtual ~VKRayTracingShaderTable();
	VkBuffer Buffer;
	bsize Size;
private:
	VkDeviceMemory Memory;

};