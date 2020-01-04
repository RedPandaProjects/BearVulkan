#pragma once
class VKShader :public BearRHI::BearRHIShader
{
	BEAR_CLASS_WITHOUT_COPY(VKShader);
public:
	VKShader(BearShaderType type);
	virtual ~VKShader();
#ifdef DEVELOPER_VERSION
	virtual bool LoadAsText(const bchar* Text, const BearMap<BearString, BearString>& Defines, BearString& OutError, BearIncluder* Includer = 0);
	virtual void* GetPointer();
	virtual	bsize GetSize();
#endif
	virtual	void LoadAsBinary(void* data, bsize size);
	BearMemoryStream ShaderOnMemory;
	inline bool IsType(BearShaderType type)const { return Type == type; }
private:
	BearShaderType Type;
};