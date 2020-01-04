#include "VKPCH.h"

VKShader::VKShader(BearShaderType type):Type(type)
{
}

VKShader::~VKShader()
{
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
#ifdef _DEBUG
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_zero);
#else
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_performance);
#endif
#ifdef DEBUG
	shaderc_compile_options_set_generate_debug_info(options);
#endif
	shaderc_shader_kind shader_kind;
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
	shaderc_compile_options_set_include_callbacks(options, &CallbackIncluder,&CallbackInclduerRelease,reinterpret_cast<void*>(Includer));
	for (auto b = Defines.begin(), e = Defines.end(); b != e; b++)
	{
#ifdef UNICODE
		shaderc_compile_options_add_macro_definition(options, *BearEncoding::FastToAnsi(*b->first), b->first.size(), *BearEncoding::FastToAnsi(*b->second), b->second.size());

#else
		shaderc_compile_options_add_macro_definition(options, *b->first, b->first.size(), *b->second, b->second.size());
#endif
	}
#ifdef UNICODE
	shaderc_compilation_result_t result = shaderc_compile_into_spv( compiler, "#version 450\nvoid main() {}", 27, shader_kind, "main.vert", "main", options);
#else
#endif
	shaderc_compile_options_release(options);
	shaderc_result_release(result);
	shaderc_compiler_release(compiler);
	return false;
}

void* VKShader::GetPointer()
{
	return ShaderOnMemory.Begin();
}

bsize VKShader::GetSize()
{
	return ShaderOnMemory.Size();
}
#endif
void VKShader::LoadAsBinary(void* data, bsize size)
{
	ShaderOnMemory.Clear();
	ShaderOnMemory.WriteBuffer(data, size);
}
