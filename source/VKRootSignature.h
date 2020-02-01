#pragma once
class VKRootSignature :public BearRHI::BearRHIRootSignature
{
	BEAR_CLASS_WITHOUT_COPY(VKRootSignature);
public:
	VKRootSignature(const BearRootSignatureDescription& desc);
	virtual ~VKRootSignature();
	bsize CountBuffers;
	bsize CountSRVs;
	bsize CountSamplers;
	VkPipelineLayout PipelineLayout;
	
	VkDescriptorSetLayout DescriptorSetLayout;

};