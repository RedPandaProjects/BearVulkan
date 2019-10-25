#pragma once
class VKRenderShader :public BearRenderBase::BearRenderShaderBase
{
	BEAR_CLASS_NO_COPY(VKRenderShader);
public:
	VKRenderShader(BearGraphics::BearShaderType type);
	virtual ~VKRenderShader();
	virtual bool CompileText(const bchar*Text, BearCore::BearString &OutError);
	VkPipelineShaderStageCreateInfo Shader;
	inline bool IsType(BearGraphics::BearShaderType type)const { return Type == type; }
private:
	BearGraphics::BearShaderType Type;
};