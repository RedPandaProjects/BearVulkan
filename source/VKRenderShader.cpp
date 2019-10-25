#include "vulkanPCH.h"
inline bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv) {
#ifndef __ANDROID__
	
	EShLanguage stage = EShLangVertex;
	glslang::TShader shader(stage);
	glslang::TProgram program;
	const char *shaderStrings[1];
	TBuiltInResource Resources = {};
	//init_resources(Resources);

	// Enable SPIR-V and Vulkan rules when parsing GLSL
	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

	shaderStrings[0] = pshader;
	shader.setStrings(shaderStrings, 1);

	if (!shader.parse(&Resources, 100, false, messages)) {
		puts(shader.getInfoLog());
		puts(shader.getInfoDebugLog());
		return false;  // something didn't work
	}

	program.addShader(&shader);

	//
	// Program-level processing...
	//

	if (!program.link(messages)) {
		puts(shader.getInfoLog());
		puts(shader.getInfoDebugLog());
		fflush(stdout);
		return false;
	}

	glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
#else
	// On Android, use shaderc instead.
	shaderc::Compiler compiler;
	shaderc::SpvCompilationResult module =
		compiler.CompileGlslToSpv(pshader, strlen(pshader), MapShadercType(shader_type), "shader");
	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		LOGE("Error: Id=%d, Msg=%s", module.GetCompilationStatus(), module.GetErrorMessage().c_str());
		return false;
	}
	spirv.assign(module.cbegin(), module.cend());
#endif
	return true;
}


VKRenderShader::VKRenderShader(BearGraphics::BearShaderType type) :Type(type), Shader(0)

{

}

VKRenderShader::~VKRenderShader()
{
	if (Shader)vkDestroyShaderModule(Factory->Device, Shader, 0);
}

bool VKRenderShader::CompileText(const bchar * Text, BearCore::BearString & OutError)
{
	//GLStoSpn();
	return false;
}
