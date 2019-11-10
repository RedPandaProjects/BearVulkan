#pragma once
class VKRenderPipeline :public BearRenderBase::BearRenderPipelineBase
{
	BEAR_CLASS_NO_COPY(VKRenderPipeline);
public:
	VKRenderPipeline(const BearGraphics::BearRenderPipelineDescription&desc);
	virtual ~VKRenderPipeline();
	virtual void Set(void*cmdlist);
	VkPipeline Pipeline;
	BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderRootSignatureBase> RootSignature;
	VKRenderRootSignature *RootSignaturePointer;
};