#pragma once
class VKDescriptorHeap :public BearRHI::BearRHIDescriptorHeap
{
	BEAR_CLASS_WITHOUT_COPY(VKDescriptorHeap);
public:
	VKDescriptorHeap(const BearDescriptorHeapDescription& desc);
	virtual ~VKDescriptorHeap();
	void Set(VkCommandBuffer  CommandLine);
	VkDescriptorSet DescriptorSet;
	BearFactoryPointer<BearRHI::BearRHIRootSignature> RootSignature;
	VKRootSignature* RootSignaturePointer;
	bsize CountBuffers;
};