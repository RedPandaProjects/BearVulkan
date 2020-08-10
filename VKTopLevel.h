#pragma once
class VKTopLevel : public VKShaderResource, public BearRHI::BearRHITopLevel
{
public:
	VKTopLevel(const BearTopLevelDescription& desc);
	virtual ~VKTopLevel();
	virtual void* QueryInterface(int Type);
	VkAccelerationStructureNV AccelerationStructure;
	VkDeviceMemory ResultBufferMemory;
private:
	VkWriteDescriptorSetAccelerationStructureNV DescriptorAccelerationStructureInfo;
	virtual void SetAsSRV(VkWriteDescriptorSet* HEAP, size_t offset);
};

