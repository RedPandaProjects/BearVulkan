#pragma once
class VKShader :public BearRHI::BearRHIShader
{
	//BEAR_CLASS_WITHOUT_COPY(VKShader);
public:
	VKShader(BearShaderType type);
	virtual ~VKShader();
#ifdef DEVELOPER_VERSION	
	virtual bool LoadAsText(const bchar* Text, const BearMap<BearStringConteniar, BearStringConteniar>& Defines, BearString& OutError, BearIncluder* Includer);

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
};