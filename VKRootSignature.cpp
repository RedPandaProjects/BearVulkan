#include "VKPCH.h"
size_t RootSignatureCounter = 0;
inline VkShaderStageFlagBits TransletionShaderVisible(BearShaderType Type)
{
	switch (Type)
	{
	case ST_Vertex:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case ST_Hull:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case ST_Domain:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case ST_Geometry:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case ST_Pixel:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case ST_Compute:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	case ST_RayGeneration:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_NV;
		break;
	case ST_Miss:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_MISS_BIT_NV;
		break;
	case ST_Callable:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_CALLABLE_BIT_NV;
		break;
	case ST_Intersection:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_INTERSECTION_BIT_NV;
		break;
	case ST_AnyHit:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_ANY_HIT_BIT_NV;
		break;
	case ST_ClosestHit:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
		break;
	case ST_ALL:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		break;
	default:
		BEAR_CHECK(0);
	}
	return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
}
VKRootSignature::VKRootSignature(const BearRootSignatureDescription& Description):Description(Description)
{

	RootSignatureCounter++;
	CountBuffers = 0;
	CountSamplers = 0;
	CountSRVs = 0;
	CountUAVs = 0;
	{
		for (size_t i = 0; i < 16; i++)
		{
			SlotSamplers[i] = 16;
			SlotBuffers[i] = 16;
			SlotSRVs[i] = 16;
			SlotUAVs[i] = 16;
			if (Description.UniformBuffers[i].Shader != ST_Null)CountBuffers++;
			if (Description.SRVResources[CountSRVs].Shader != ST_Null) CountSRVs++;
			if (Description.UAVResources[CountUAVs].Shader != ST_Null) CountUAVs++;
			if (Description.Samplers[CountSamplers].Shader != ST_Null) CountSamplers++;
		}
	}
	
	{
		size_t Offset = 0;
		VkDescriptorSetLayoutBinding LayoutBinding[64];
		{

			for (size_t i = 0; i < 16; i++)
			{
				if (Description.UniformBuffers[i].Shader != ST_Null)
				{
					SlotBuffers[i] = Offset;
					LayoutBinding[Offset].binding = static_cast<uint32_t>(i);
					LayoutBinding[Offset].descriptorCount = 1;
					LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					LayoutBinding[Offset].pImmutableSamplers = nullptr;
					LayoutBinding[Offset].stageFlags = TransletionShaderVisible(Description.UniformBuffers[i].Shader);
					Offset++;
				}
			}
			for (size_t i = 0; i < 16; i++)
			{
				if (Description.SRVResources[i].Shader != ST_Null)
				{
					SlotSRVs[i] = Offset - CountBuffers;
					LayoutBinding[Offset].binding = static_cast<uint32_t>(i + 16);
					LayoutBinding[Offset].descriptorCount = 1;
					switch (Description.SRVResources[i].DescriptorType)
					{
					case BearDescriptorType::DT_Buffer:
						LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						break;
					case BearDescriptorType::DT_Image:
						LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						break;
					case BearDescriptorType::DT_AccelerationStructure:
						LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
						break;
					default:
						BEAR_CHECK(0);
						break;
					}
					LayoutBinding[Offset].pImmutableSamplers = nullptr;
					LayoutBinding[Offset].stageFlags = TransletionShaderVisible(Description.SRVResources[i].Shader);
					Offset++;
				}
			
			}


			for (size_t i = 0; i < 16; i++)
			{
				if (Description.UAVResources[i].Shader != ST_Null)
				{
					SlotUAVs[i] = Offset - (CountBuffers + CountSRVs);
					LayoutBinding[Offset].binding = static_cast<uint32_t>(i + 32);
					LayoutBinding[Offset].descriptorCount = 1;
					switch (Description.UAVResources[i].DescriptorType)
					{
					case BearDescriptorType::DT_Buffer:
						LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						break;
					case BearDescriptorType::DT_Image:
						LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						break;
					default:
						BEAR_CHECK(0);
						break;
					}
					LayoutBinding[Offset].pImmutableSamplers = nullptr;
					LayoutBinding[Offset].stageFlags = TransletionShaderVisible(Description.UAVResources[i].Shader);
					Offset++;
				}

			}

			for (size_t i = 0; i < 16; i++)
			{
				if (Description.Samplers[i].Shader != ST_Null)
				{
					SlotSamplers[i] = Offset - (CountBuffers+CountSRVs+ CountUAVs);
					LayoutBinding[Offset].binding = static_cast<uint32_t>(i + 48);
					LayoutBinding[Offset].descriptorCount = 1;
					LayoutBinding[Offset].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
					LayoutBinding[Offset].pImmutableSamplers = nullptr;
					LayoutBinding[Offset].stageFlags = TransletionShaderVisible(Description.Samplers[i].Shader);
					Offset++;
				}
			}
		}

		VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
		LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutCreateInfo.bindingCount = static_cast<uint32_t>(Offset);;
		LayoutCreateInfo.pBindings = LayoutBinding;

		V_CHK(vkCreateDescriptorSetLayout(Factory->Device, &LayoutCreateInfo, nullptr, &DescriptorSetLayout));
	}
	{
		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
		PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutCreateInfo.pNext = NULL;
		PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		PipelineLayoutCreateInfo.pPushConstantRanges = NULL;
		PipelineLayoutCreateInfo.setLayoutCount = 1;
		PipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;

		V_CHK(vkCreatePipelineLayout(Factory->Device, &PipelineLayoutCreateInfo, NULL, &PipelineLayout));
	}
		

}

VKRootSignature::~VKRootSignature()
{
	RootSignatureCounter--;
	vkDestroyPipelineLayout(Factory->Device, PipelineLayout, 0);
	vkDestroyDescriptorSetLayout(Factory->Device, DescriptorSetLayout, 0);
}

