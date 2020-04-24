#pragma once
class VKPipelineMesh :public BearRHI::BearRHIPipelineMesh,public VKPipeline
{
	//BEAR_CLASS_WITHOUT_COPY(VKPipelineGraphics);
public:
	VKPipelineMesh(const BearPipelineMeshDescription&desc);
	virtual ~VKPipelineMesh();
	virtual void Set(VkCommandBuffer CommandBuffer);
	virtual void* QueryInterface(int Type);
	virtual BearPipelineType GetType();
	VkPipeline Pipeline;
	BearFactoryPointer<BearRHI::BearRHIRootSignature> RootSignature;
	VKRootSignature* RootSignaturePointer;
};