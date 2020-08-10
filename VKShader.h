#pragma once
class VKShader :public BearRHI::BearRHIShader
{
	//BEAR_CLASS_WITHOUT_COPY(VKShader);
public:
	VKShader(BearShaderType type);
	virtual ~VKShader();
#ifdef DEVELOPER_VERSION	
	virtual bool LoadAsText(const bchar* Text, const bchar* EntryPoint, const BearMap<BearStringConteniar, BearStringConteniar>& Defines, BearString& OutError, BearIncluder* Includer);
private:
	virtual bool LoadAsTextShaderc(const bchar* Text, const bchar* EntryPoint, const BearMap<BearStringConteniar, BearStringConteniar>& Defines, BearString& OutError, BearIncluder* Includer);
#ifdef RTX
	virtual bool LoadAsTextDXC(const bchar* Text, const bchar* EntryPoint, const BearMap<BearStringConteniar, BearStringConteniar>& Defines, BearString& OutError, BearIncluder* Includer);
#endif
public:
	virtual void* GetPointer();
	virtual	size_t GetSize();
#endif
	virtual	void LoadAsBinary(void* data, size_t size);
	BearVector<uint32> ShaderOnMemory;
	inline bool IsType(BearShaderType type)const { return Type == type; }
	VkPipelineShaderStageCreateInfo Shader;
private:
	void CreateShader();
	BearShaderType Type;
	BearStringConteniarAnsi EntryPointName;
};