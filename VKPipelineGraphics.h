#pragma once
class VKPipelineGraphics :public BearRHI::BearRHIPipelineGraphics,public VKPipeline
{
	//BEAR_CLASS_WITHOUT_COPY(VKPipelineGraphics);
public:
	VKPipelineGraphics(const BearPipelineGraphicsDescription&desc);
	virtual ~VKPipelineGraphics();
	virtual void Set(VkCommandBuffer CommandBuffer);
	virtual void* QueryInterface(int Type);
	virtual BearPipelineType GetType();
	VkPipeline Pipeline;
	BearFactoryPointer<BearRHI::BearRHIRootSignature> RootSignature;
	VKRootSignature* RootSignaturePointer;
};