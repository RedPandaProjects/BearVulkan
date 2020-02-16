#include "VKPCH.h"
bsize RootSignatureCounter = 0;
inline VkShaderStageFlagBits TransletionShaderVisible(BearShaderType Type)
{
	switch (Type)
	{
	case ST_Vertex:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

		break;
	case ST_Pixel:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		BEAR_RASSERT(0);
	}
	return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
}
VKRootSignature::VKRootSignature(const BearRootSignatureDescription& Description)
{
	RootSignatureCounter++;
	CountBuffers = 0;
	CountSamplers = 0;
	CountSRVs = 0;
	{
		for (; CountBuffers < 16 && Description.UniformBuffers[CountBuffers].Shader != ST_Null; CountBuffers++);
		for (; CountSRVs < 16 && Description.SRVResources[CountSRVs].Shader != ST_Null; CountSRVs++);
		for (; CountSamplers < 16 && Description.Samplers[CountSamplers].Shader != ST_Null; CountSamplers++);
	}
	
	bsize Count = CountBuffers+ CountSRVs+ CountSamplers;
	{
		bsize Offset = 0;
		VkDescriptorSetLayoutBinding LayoutBinding[64];
		{

			for (bsize i = 0; i < CountSRVs; i++)
			{
				LayoutBinding[i + Offset].binding = static_cast<uint32_t>(i + Offset);
				LayoutBinding[i + Offset].descriptorCount = 1;
				LayoutBinding[i + Offset].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				LayoutBinding[i + Offset].pImmutableSamplers = nullptr;
				LayoutBinding[i + Offset].stageFlags = TransletionShaderVisible(Description.SRVResources[i].Shader);
			}
			Offset += CountSRVs;
			for (bsize i = 0; i < CountSamplers; i++)
			{
				LayoutBinding[i + Offset].binding = static_cast<uint32_t>(i + Offset);
				LayoutBinding[i + Offset].descriptorCount = 1;
				LayoutBinding[i + Offset].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				LayoutBinding[i + Offset].pImmutableSamplers = nullptr;
				LayoutBinding[i + Offset].stageFlags = TransletionShaderVisible(Description.Samplers[i].Shader);
			
			}

			Offset += CountSamplers;
			for (bsize i = 0; i < CountBuffers; i++)
			{
				LayoutBinding[i + Offset].binding = static_cast<uint32_t>(i + Offset);
				LayoutBinding[i + Offset].descriptorCount = 1;
				LayoutBinding[i + Offset].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				LayoutBinding[i + Offset].pImmutableSamplers = nullptr;
				LayoutBinding[i + Offset].stageFlags = TransletionShaderVisible(Description.UniformBuffers[i].Shader);
			}
		}

		VkDescriptorSetLayoutCreateInfo LayoutCreateInfo = {};
		LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutCreateInfo.bindingCount = static_cast<uint32_t>(Count);;
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

