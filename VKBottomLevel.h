#pragma once
class VKBottomLevel : public BearRHI::BearRHIBottomLevel
{
public:
	VKBottomLevel(const BearBottomLevelDescription& desc);
	virtual ~VKBottomLevel();
	virtual void* QueryInterface(int Type);
	VkAccelerationStructureNV AccelerationStructure;
	VkDeviceMemory ResultBufferMemory;
	uint64_t AccelerationStructureHandle;
};

