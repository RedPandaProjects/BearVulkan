#pragma once
class VKDescriptorHeap :public BearRHI::BearRHIDescriptorHeap
{
	BEAR_CLASS_WITHOUT_COPY(VKDescriptorHeap);
public:
	VKDescriptorHeap(const BearDescriptorHeapDescription& desc);
	virtual ~VKDescriptorHeap();
	void Set(VkCommandBuffer  CommandLine);
	virtual void SetUniformBuffer(bsize slot, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> UniformBuffer);
	virtual	void SetShaderResource(bsize slot, BearFactoryPointer<BearRHI::BearRHIShaderResource> ShaderResource);
	virtual	void SetSampler(bsize slot, BearFactoryPointer<BearRHI::BearRHISampler> Sampler);
	VkDescriptorSet DescriptorSet;
	BearFactoryPointer<BearRHI::BearRHIRootSignature> RootSignature;
	VKRootSignature* RootSignaturePointer;
	VkDescriptorPool DescriptorPool;
	BearFactoryPointer<BearRHI::BearRHIUniformBuffer> UniformBuffers[16];
	bsize CountBuffers;
	BearFactoryPointer<BearRHI::BearRHIShaderResource> ShaderResources[16];
	bsize CountSRVs;
	BearFactoryPointer<BearRHI::BearRHISampler> Samplers[16];
	bsize CountSamplers;
};