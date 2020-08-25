#pragma once
class VKRayTracingShaderTable :public BearRHI::BearRHIRayTracingShaderTable
{
	//BEAR_CLASS_WITHOUT_COPY(VKUniformBuffer);
public:
	VKRayTracingShaderTable(const BearRayTracingShaderTableDescription& description);
	virtual ~VKRayTracingShaderTable();
	
	struct ShaderRecord
	{
		ShaderRecord():Buffer(nullptr), Memory(nullptr),Stride(0){}
		VkBuffer Buffer;
		VkDeviceMemory Memory;
		bsize Stride;
	} RayGenerateRecord,MissRecord,HitGroups, CallableRecord;
};