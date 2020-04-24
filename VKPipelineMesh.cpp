#include "VKPCH.h"
#pragma optimize( "", off )
size_t PipelineMeshCounter = 0;
inline VkFormat TranslateVertexFormat(BearVertexFormat format)
{
	switch (format)
	{
	case VF_R16G16_SINT:
		return VkFormat::VK_FORMAT_R16G16_SINT;
	case VF_R16G16B16A16_SINT:
		return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
	case VF_R16G16_FLOAT:
		return VkFormat::VK_FORMAT_R16G16_SFLOAT;
	case VF_R16G16B16A16_FLOAT:
		return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
	case VF_R32G32B32A32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
	case VF_R32G32B32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
	case VF_R32G32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32_SFLOAT;
	case VF_R32_FLOAT:
		return VkFormat::VK_FORMAT_R32_SFLOAT;

	case VF_R32_INT:
		return VkFormat::VK_FORMAT_R32_SINT;
	case VF_R8G8B8A8:
		return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
	case VF_R8G8:
		return VkFormat::VK_FORMAT_R8G8_UINT;
	case VF_R8:
		return VkFormat::VK_FORMAT_R8_UINT;
	default:
		BEAR_CHECK(0);;
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	//return DXGI_FORMAT_UNKNOWN;
}
inline size_t VertexFormatToSize(BearVertexFormat format)
{
	switch (format)
	{
	case VF_R16G16_SINT:
		return 2 * 2;
	case VF_R16G16B16A16_SINT:
		return 2 * 4;
	case VF_R16G16_FLOAT:
		return 2 * 2;
	case VF_R16G16B16A16_FLOAT:
		return 2 * 4;

	case VF_R32G32B32A32_FLOAT:
		return sizeof(float) * 4;
	case VF_R32G32B32_FLOAT:
		return sizeof(float) * 3;
	case VF_R32G32_FLOAT:
		return sizeof(float) * 2;
	case VF_R32_FLOAT:
		return sizeof(float);

	case VF_R32_INT:
		return sizeof(int);
	case VF_R8G8B8A8:
		return sizeof(int);

	case VF_R8G8:
		return sizeof(short);
	case VF_R8:
		return sizeof(uint8);
	default:
		BEAR_CHECK(0);;
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	//return DXGI_FORMAT_UNKNOWN;
}
VKPipelineMesh::VKPipelineMesh(const BearPipelineMeshDescription& desc)
{
	PipelineMeshCounter++;

	Pipeline = 0;
}

VKPipelineMesh::~VKPipelineMesh()
{
	PipelineMeshCounter--;
}

void VKPipelineMesh::Set(VkCommandBuffer CommandBuffer)
{
}

void* VKPipelineMesh::QueryInterface(int Type)
{
	switch (Type)
	{
	case VKQ_Pipeline:
		return reinterpret_cast<void*>(static_cast<VKPipeline*>(this));
	default:
		return nullptr;
	}
}

BearPipelineType VKPipelineMesh::GetType()
{
	return PT_Mesh;
}
