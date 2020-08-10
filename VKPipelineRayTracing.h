#pragma once
class VKPipelineRayTracing :public BearRHI::BearRHIPipelineRayTracing,public VKPipeline
{
	//BEAR_CLASS_WITHOUT_COPY(VKPipelineGraphics);
public:
	VKPipelineRayTracing(const BearPipelineRayTracingDescription&desc);
	virtual ~VKPipelineRayTracing();
	virtual void Set(VkCommandBuffer CommandBuffer);
	virtual void* QueryInterface(int Type);
	virtual BearPipelineType GetType();
	void* GetShaderIdentifier(BearStringConteniarUnicode name);
#ifdef RTX
	BearMap<BearStringConteniarUnicode, bsize> GroupMap;
	VkPipeline Pipeline;
	BearFactoryPointer<BearRHI::BearRHIRootSignature> RootSignature;
	VKRootSignature* RootSignaturePointer;
	BearRef<uint8> ShaderIdentifiers;
#endif
};