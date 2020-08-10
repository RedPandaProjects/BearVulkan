#include "VKPCH.h"
#ifdef RTX

struct DXCInluder :public IDxcIncludeHandler
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override {
		return E_FAIL;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		return 0;
	}
	wchar_t GPath[1024];
	wchar_t LPath[1024];
	BearVector<BearRef<BearBufferedReader>> Readers;
	BearIncluder* m_Includer;
	BearVector<IDxcBlobEncoding*> BlobEncodings;
	DXCInluder(BearIncluder* Includer) :m_Includer(Includer) {}
	~DXCInluder()
	{

	}
	virtual HRESULT STDMETHODCALLTYPE LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource)
	{
		if (m_Includer == nullptr)return E_FAIL;

		BearStringPath Name;

		if (BearString::ExistPossition(pFilename, 0, L".///"))
		{
			pFilename += 4;
		}
		if (BearString::ExistPossition(pFilename, 0, L"./"))
		{
			pFilename += 2;
		}

		BearString::Copy(Name,
#ifndef UNICODE
			* BearEncoding::FastToAnsi(pFilename)
#else
			pFilename
#endif
		);

		auto steam = m_Includer->OpenAsBuffer(Name);

		if (!*steam)return E_FAIL;
		IDxcBlobEncoding* PointerTextBlob;
		bool bIsUTF8 = false;

		if (steam->Size() > 2)
		{
			char utf8_bom[3];
			steam->ReadBuffer(utf8_bom, 3);
			bIsUTF8 = utf8_bom[0] == 0xEF;
			bIsUTF8 = bIsUTF8 && utf8_bom[1] == 0xBB;
			bIsUTF8 = bIsUTF8 && utf8_bom[2] == 0xBF;
			steam->Seek(0);
		}

		BEAR_ASSERT(SUCCEEDED(Factory->DxcLibrary->CreateBlobWithEncodingFromPinned(steam->Begin(), static_cast<UINT32>(steam->Size()), bIsUTF8 ? DXC_CP_UTF8 : DXC_CP_ACP, &PointerTextBlob)));
		*ppIncludeSource = static_cast<IDxcBlob*>(PointerTextBlob);
		Readers.push_back(steam);
		BlobEncodings.push_back(PointerTextBlob);
		return S_OK;
	}

};

extern bool GDebugRender;
bool VKShader::LoadAsTextDXC(const bchar* Text, const bchar* EntryPoint, const BearMap<BearStringConteniar, BearStringConteniar>& Defines, BearString& OutError, BearIncluder* Includer)
{
	bool bIsUTF8 = false;
	if (BearString::GetSize(Text) > 2)
	{
		bIsUTF8 = Text[0] == 0xEF;
		bIsUTF8 = bIsUTF8 && Text[1] == 0xBB;
		bIsUTF8 = bIsUTF8 && Text[2] == 0xBF;

	}

	IDxcResult* Result;
	DXCInluder LIncluder(Includer);
	wchar_t NameFile[1024];
	swprintf(NameFile, 1024, L"%S", "noname");
	BearVector<const wchar_t*> Arguments;
	BearVector<wchar_t*> StringForDelete;
	Arguments.push_back(L"-spirv");
	Arguments.push_back(L"-fvk-use-scalar-layout"); 	
	if (GDebugRender)
	{
		Arguments.push_back(L"-Od");
	}
	else
	{
	}
	switch (Type)
	{

	case ST_RayGeneration:
	case ST_Miss:
	case ST_Callable:
	case ST_Intersection:
	case ST_ClosestHit:
	case ST_AnyHit:
		Arguments.push_back(L"-fspv-extension=SPV_NV_ray_tracing");
		Arguments.push_back(L"-Tlib_6_3");
		break;
	default:
		BEAR_CHECK(false);
		break;
	}

	Arguments.push_back(L"-DENABLE_SPIRV_CODEGEN=ON");
	Arguments.push_back(L"-D");
	Arguments.push_back(L"VULKAN=1");

	{

		for (auto b = Defines.begin(), e = Defines.end(); b != e; b++)
		{


			BearString Temp;
			Temp.append(*b->first);
			Temp.append(TEXT("="));
			Temp.append(*b->second);
#ifdef UNICODE

			StringForDelete.push_back(BearString::Duplicate(*Temp));
#else
			StringForDelete.push_back(BearString::Duplicate(*BearEncoding::FastToUnicode(*Temp)));
#endif

			Arguments.push_back(L"-D");
			Arguments.push_back(StringForDelete.back());
		}
	}
	//DXCInluder Includer;
	DxcBuffer Buffer;
	Buffer.Ptr = Text;
	Buffer.Size = BearString::GetSize(Text);
	Buffer.Encoding = bIsUTF8 ? DXC_CP_UTF8 : DXC_CP_ACP;

	BEAR_ASSERT(SUCCEEDED(Factory->DxcCompiler->Compile(&Buffer, (LPCWSTR*)Arguments.data(), static_cast<UINT32>(Arguments.size()), &LIncluder, IID_PPV_ARGS(&Result))));

	for (bsize i = 0; i < StringForDelete.size(); i++)
	{
		bear_free(StringForDelete[i]);
	}
	HRESULT ResultCode;
	BEAR_ASSERT(SUCCEEDED(Result->GetStatus(&ResultCode)));
	if (FAILED(ResultCode))
	{
		IDxcBlobEncoding* PError;
		BEAR_ASSERT(SUCCEEDED(Result->GetErrorBuffer(&PError)));

		BearVector<char> infoLog(PError->GetBufferSize() + 1);
		memcpy(infoLog.data(), PError->GetBufferPointer(), PError->GetBufferSize());
		infoLog[PError->GetBufferSize()] = 0;

		BearStringAnsi errorMsg = "Shader Compiler Error:\n";
		errorMsg.append(infoLog.data());
		PError->Release();
#ifdef UNICODE
		OutError.assign(*BearEncoding::FastToUnicode(*errorMsg));
#else
		OutError.assign(*errorMsg);
#endif
		return false;
	}

	IDxcBlob* Shader;
	IDxcBlobUtf16* pShaderName = nullptr;
	BEAR_ASSERT(SUCCEEDED(Result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&Shader), &pShaderName)));
	BEAR_CHECK(Shader->GetBufferSize() % 4 == 0);
	ShaderOnMemory.resize(Shader->GetBufferSize() / 4);
	bear_copy(ShaderOnMemory.data(), Shader->GetBufferPointer(), Shader->GetBufferSize());
	Shader->Release();
	if(pShaderName)
		pShaderName->Release();
	Result->Release();
	CreateShader();
	this->Shader.pName = EntryPoint;
	return true;
}
#endif