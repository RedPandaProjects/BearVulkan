#pragma once
class VKShader :public BearRHI::BearRHIShader
{
	//BEAR_CLASS_WITHOUT_COPY(VKShader);
public:
	VKShader(BearShaderType type);
	virtual ~VKShader();
#ifdef DEVELOPER_VERSION	
	virtual bool LoadAsText(const bchar* text, const bchar* entry_point, const BearMap<BearStringConteniar, BearStringConteniar>& defines, BearString& out_error, BearIncluder* includer);
private:
	virtual bool LoadAsTextShaderc(const bchar* text, const bchar* entry_point, const BearMap<BearStringConteniar, BearStringConteniar>& defines, BearString& out_error, BearIncluder* includer);
#ifdef RTX
	virtual bool LoadAsTextDXC(const bchar* text, const bchar* entry_point, const BearMap<BearStringConteniar, BearStringConteniar>& defines, BearString& out_error, BearIncluder* includer);
#endif
public:
	virtual void* GetPointer();
	virtual	size_t GetSize();
#endif
	virtual	void LoadAsBinary(void* data, size_t size);
	BearVector<uint32> ShaderOnMemory;
	inline bool IsType(BearShaderType type)const { return m_Type == type; }
	VkPipelineShaderStageCreateInfo Shader;
private:
	void CreateShader();
	BearShaderType m_Type;
	BearStringConteniarAnsi m_EntryPointName;
};