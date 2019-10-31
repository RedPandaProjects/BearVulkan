#pragma once
class VKRenderRootSignature :public BearRenderBase::BearRenderRootSignatureBase
{
	BEAR_CLASS_NO_COPY(VKRenderRootSignature);
public:
	VKRenderRootSignature(const BearGraphics::BearRenderRootSignatureDescription&desc);
	virtual ~VKRenderRootSignature();
	void Set(VkCommandBuffer  CommandLine);
	VkDescriptorSetLayout DescriptorSetLayout;
	VkPipelineLayout PipelineLayout;
	VkDescriptorPool DescriptorPool;
	VkDescriptorSet DescriptorSet;
};