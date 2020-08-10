#pragma once
class VKDescriptorHeap :public BearRHI::BearRHIDescriptorHeap
{
	//BEAR_CLASS_WITHOUT_COPY(VKDescriptorHeap);
public:
	VKDescriptorHeap(const BearDescriptorHeapDescription& desc);
	virtual ~VKDescriptorHeap();
	virtual void SetUniformBuffer(size_t slot, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> UniformBuffer, size_t offset = 0);
	virtual	void SetShaderResource(size_t slot, BearFactoryPointer<BearRHI::BearRHIShaderResource> ShaderResource, size_t offset = 0);
	virtual	void SetSampler(size_t slot, BearFactoryPointer<BearRHI::BearRHISampler> Sampler);
	virtual void SetUnorderedAccess(bsize slot, BearFactoryPointer<BearRHI::BearRHIUnorderedAccess> UnorderedAccess, bsize offset = 0);

	VkDescriptorSet DescriptorSet;
	BearFactoryPointer<BearRHI::BearRHIRootSignature> RootSignature;
	VKRootSignature* RootSignaturePointer;
	VkDescriptorPool DescriptorPool;


	BearFactoryPointer<BearRHI::BearRHIUniformBuffer> UniformBuffers[16];
	size_t UniformBufferOffsets[16];
	size_t ShaderResourcesOffsets[16];
	size_t UnorderedAccessOffsets[16];

	size_t CountBuffers;
	BearFactoryPointer<BearRHI::BearRHIShaderResource> ShaderResources[16];
	size_t CountSRVs;
	BearFactoryPointer<BearRHI::BearRHISampler> Samplers[16];
	size_t CountSamplers;
	BearFactoryPointer<BearRHI::BearRHIUnorderedAccess> UnorderedAccess[16];
	size_t CountUAVs;

	size_t SlotBuffers[16];
	size_t SlotSRVs[16];
	size_t SlotSamplers[16];
	size_t SlotUAVs[16];

	void SetGraphics(VkCommandBuffer  CommandLine);
	void SetRayTracing(VkCommandBuffer  CommandLine);
};