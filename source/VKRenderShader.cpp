#include "vulkanPCH.h"
extern bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv,BearString&out);


VKRenderShader::VKRenderShader(BearGraphics::BearShaderType type) :Type(type)

{
	Shader.module = 0;
}

VKRenderShader::~VKRenderShader()
{
	if (Shader.module)vkDestroyShaderModule(Factory->Device, Shader.module, 0);
}

bool VKRenderShader::CompileText(const bchar * text, BearCore::BearString & OutError)
{
	bool  retVal;
	std::vector<unsigned int> vtx_spv;
	Shader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	Shader.pNext = NULL;
	Shader.pSpecializationInfo = NULL;
	Shader.flags = 0;
	switch (Type)
	{
	case BearGraphics::ST_Vertex:
		Shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case BearGraphics::ST_Pixel:
		Shader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		Shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	}

	Shader.pName = "main";

	retVal = GLSLtoSPV(Shader.stage, *BearCore::BearEncoding::ToANSI(text), vtx_spv, OutError);
	assert(retVal);
	VkShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
	moduleCreateInfo.pCode = vtx_spv.data();
	V_CHK(  vkCreateShaderModule(Factory->Device, &moduleCreateInfo, NULL, &Shader.module));

	return true;
}
