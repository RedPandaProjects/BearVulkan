#include "VKPCH.h"
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
	CountBuffers = 0;
	{
		for (; CountBuffers < 16 && Description.UniformBuffers[CountBuffers].Shader != ST_Null; CountBuffers++);
	}
	
	bsize Count = CountBuffers;
	{
		bsize Offset = 0;
		VkDescriptorSetLayoutBinding LayoutBinding[64];
		{
			for (bsize i = 0; i < CountBuffers; i++)
			{
				LayoutBinding[i + Offset].binding = static_cast<uint32_t>(i + Offset);
				LayoutBinding[i + Offset].descriptorCount = 1;
				LayoutBinding[i + Offset].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				LayoutBinding[i + Offset].pImmutableSamplers = nullptr;
				LayoutBinding[i + Offset].stageFlags = TransletionShaderVisible(Description.UniformBuffers[i].Shader);
			}
			Offset += CountBuffers;
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
	{
		VkDescriptorPoolSize PoolSizes[64];
		bsize Offset = 0;
		for (bsize i = 0; i < CountBuffers; i++)
		{
			PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			PoolSizes[i + Offset].descriptorCount = 1;
		}
		Offset += CountBuffers;

		VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
		DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.pNext = NULL;
		DescriptorPoolCreateInfo.maxSets = 1;
		DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(Count);;;
		DescriptorPoolCreateInfo.pPoolSizes = PoolSizes;

		V_CHK(vkCreateDescriptorPool(Factory->Device, &DescriptorPoolCreateInfo, NULL, &DescriptorPool));
	}

}

VKRootSignature::~VKRootSignature()
{

	vkDestroyDescriptorPool(Factory->Device, DescriptorPool, 0);
	vkDestroyPipelineLayout(Factory->Device, PipelineLayout, 0);
	vkDestroyDescriptorSetLayout(Factory->Device, DescriptorSetLayout, 0);
}

