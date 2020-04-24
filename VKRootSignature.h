#pragma once
class VKRootSignature :public BearRHI::BearRHIRootSignature
{
	//BEAR_CLASS_WITHOUT_COPY(VKRootSignature);
public:
	VKRootSignature(const BearRootSignatureDescription& desc);
	virtual ~VKRootSignature();
	size_t CountBuffers;
	size_t CountSRVs;
	size_t CountSamplers;
	size_t SlotBuffers[16];
	size_t SlotSRVs[16];
	size_t SlotSamplers[16];
	VkPipelineLayout PipelineLayout;
	BearRootSignatureDescription Description;
	VkDescriptorSetLayout DescriptorSetLayout;

};