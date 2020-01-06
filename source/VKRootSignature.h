#pragma once
class VKRootSignature :public BearRHI::BearRHIRootSignature
{
	BEAR_CLASS_WITHOUT_COPY(VKRootSignature);
public:
	VKRootSignature(const BearRootSignatureDescription& desc);
	virtual ~VKRootSignature();
	bsize CountBuffers;
	
	VkPipelineLayout PipelineLayout;
	VkDescriptorPool DescriptorPool;
	VkDescriptorSetLayout DescriptorSetLayout;

};