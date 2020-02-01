#include "VKPCH.h"
bsize ShaderCounter = 0;
VKShader::VKShader(BearShaderType type):Type(type)
{
	ShaderCounter++;
	Shader.module = 0;
}

VKShader::~VKShader()
{
	ShaderCounter--;
	if (Shader.module)vkDestroyShaderModule(Factory->Device, Shader.module, 0);
}

#ifdef DEVELOPER_VERSION
static shaderc_include_result* CallbackIncluder(void* user_data, const char* requested_source, int type,const char* requesting_source, size_t include_depth)
{
	shaderc_include_result* result = bear_alloc< shaderc_include_result>(1);
#ifdef UNICODE
	auto stream = reinterpret_cast<BearIncluder*>(user_data)->OpenAsBuffer(*BearEncoding::FastToUnicode( requested_source));
#else
	auto stream =  reinterpret_cast<BearIncluder*>(user_data)->OpenAsBuffer(requested_source);
#endif

	result->content_length = stream->Size();
	auto data = stream->Read();
	result->content = reinterpret_cast<char*>( *data);
	data.clear_no_free();
	result->source_name = requested_source;
	result->source_name_length = BearString::GetSize(requested_source);
	return result;
}
static void CallbackInclduerRelease(void* user_data, shaderc_include_result* include_result)
{
	bear_free(include_result->content);
	bear_free(include_result);
}
bool VKShader::LoadAsText(const bchar* Text, const BearMap<BearString, BearString>& Defines, BearString& OutError, BearIncluder* Includer)
{
	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
	shaderc_compile_options_set_source_language(options, shaderc_source_language_hlsl);

#ifdef VK_11
	shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
	shaderc_compile_options_set_target_spirv(options, shaderc_spirv_version_1_3);
#else
	shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
	shaderc_compile_options_set_target_spirv(options, shaderc_spirv_version_1_0);
#endif
	shaderc_compile_options_set_forced_version_profile(options, 450, shaderc_profile_core);
#ifdef _DEBUG
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_zero);
#else
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_performance);
#endif
#ifdef DEBUG
	shaderc_compile_options_set_generate_debug_info(options);
#endif
	shaderc_shader_kind shader_kind = shaderc_shader_kind::shaderc_vertex_shader;
	switch (Type)
	{
	case ST_Vertex:
		shader_kind = shaderc_shader_kind::shaderc_vertex_shader;
		break;
	case ST_Pixel:
		shader_kind = shaderc_shader_kind::shaderc_fragment_shader;
		break;
	default:
		break;
	};
	shaderc_compile_options_set_include_callbacks(options, &CallbackIncluder, &CallbackInclduerRelease, reinterpret_cast<void*>(Includer));
	for (auto b = Defines.begin(), e = Defines.end(); b != e; b++)
	{
#ifdef UNICODE
		shaderc_compile_options_add_macro_definition(options, *BearEncoding::FastToAnsi(*b->first), b->first.size(), *BearEncoding::FastToAnsi(*b->second), b->second.size());

#else
		shaderc_compile_options_add_macro_definition(options, *b->first, b->first.size(), *b->second, b->second.size());
#endif
	}
#ifdef UNICODE
	shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, *BearEncoding::FastToAnsi(Text), BearString::GetSize(Text), shader_kind, "noname", "main", options);
#else
	shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, Text, BearString::GetSize(Text), shader_kind, "noname", "main", options);
#endif

	if (shaderc_result_get_num_errors(result))
	{
		const char* text = shaderc_result_get_error_message(result);
#ifdef UNICODE
		OutError = BearEncoding::FastToUnicode(text);
#else
		OutError = text;
#endif
		shaderc_compile_options_release(options);
		shaderc_result_release(result);
		shaderc_compiler_release(compiler);
		return false;
	}
	BEAR_RASSERT(shaderc_result_get_compilation_status(result) == shaderc_compilation_status_success);
	if (shaderc_result_get_num_warnings(result))
	{
		const char* text = shaderc_result_get_error_message(result);
#ifdef UNICODE
		OutError = BearEncoding::FastToUnicode(text);
#else
		OutError = text;
#endif
	}

	ShaderOnMemory.clear();

	ShaderOnMemory.resize(shaderc_result_get_length(result)/4);
	bear_copy(ShaderOnMemory.data(), shaderc_result_get_bytes(result), shaderc_result_get_length(result));
	BEAR_ASSERT(shaderc_result_get_length(result) % 4 == 0)
	shaderc_compile_options_release(options);
	shaderc_result_release(result);
	shaderc_compiler_release(compiler);
	CreateShader();
	return true;
}

void* VKShader::GetPointer()
{
	return ShaderOnMemory.data();
}

bsize VKShader::GetSize()
{
	return ShaderOnMemory.size()*4;
}
#endif
void VKShader::LoadAsBinary(void* data, bsize size)
{
	if (size == 0)return;
	ShaderOnMemory.clear();
	BEAR_ASSERT(size % 4 == 0)
	ShaderOnMemory.resize( size/4);
	bear_copy(ShaderOnMemory.data(), data, size);
	CreateShader();
}

void VKShader::CreateShader()
{
	if (ShaderOnMemory.size() == 0)return;
	if (Shader.module)vkDestroyShaderModule(Factory->Device, Shader.module, 0);
	Shader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	Shader.pNext = NULL;
	Shader.pSpecializationInfo = NULL;
	Shader.flags = 0;
	switch (Type)
	{
	case ST_Vertex:
		Shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case ST_Pixel:
		Shader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		Shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	}

	Shader.pName = "main";

	VkShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = ShaderOnMemory.size()*sizeof(uint32);
	moduleCreateInfo.pCode = (uint32*)ShaderOnMemory.data();
	V_CHK(vkCreateShaderModule(Factory->Device, &moduleCreateInfo, NULL, &Shader.module));

}
