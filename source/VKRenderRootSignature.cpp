#include "vulkanPCH.h"
 
inline VkShaderStageFlagBits TransletionShaderVisible(BearGraphics::BearShaderType Type)
{
	switch (Type)
	{
	case BearGraphics::ST_Vertex:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

		break;
	case BearGraphics::ST_Pixel:
		return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		BEAR_RASSERT(0);
	}
	return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
}
VKRenderRootSignature::VKRenderRootSignature(const BearGraphics::BearRenderRootSignatureDescription & Description)
{
	bsize CountBuffers = 0;
	{
		//for (; CountBuffers < 16 && !Description.UniformBuffers[CountBuffers].Buffer.empty(); CountBuffers++);
	}
	bsize Count = CountBuffers;
	{
		bsize Offset = 0;
		VkDescriptorSetLayoutBinding LayoutBinding[64];
		{
			for (bsize i = 0; i < CountBuffers; i++)
			{
				LayoutBinding[i + Offset].binding = static_cast<uint32_t>(i+ Offset);
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
			PoolSizes[i+ Offset].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
	{
		VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo[1];
		DescriptorSetAllocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		DescriptorSetAllocateInfo[0].pNext = NULL;
		DescriptorSetAllocateInfo[0].descriptorPool = DescriptorPool;
		DescriptorSetAllocateInfo[0].descriptorSetCount = 1;
		DescriptorSetAllocateInfo[0].pSetLayouts =&DescriptorSetLayout;

		V_CHK(vkAllocateDescriptorSets(Factory->Device, DescriptorSetAllocateInfo, &DescriptorSet));
	

		VkWriteDescriptorSet WriteDescriptorSet[64];
		bsize Offset = 0;
		for (bsize i = 0; i < CountBuffers; i++)
		{
			/*auto *buffer = static_cast<const VKRenderUniformBuffer*>(Description.UniformBuffers[i].Buffer.get());
			WriteDescriptorSet[i + Offset] = {};
			WriteDescriptorSet[i + Offset].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			WriteDescriptorSet[i + Offset].pNext = NULL;
			WriteDescriptorSet[i + Offset].dstSet = DescriptorSet;
			WriteDescriptorSet[i + Offset].descriptorCount = 1;
			WriteDescriptorSet[i + Offset].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			WriteDescriptorSet[i + Offset].pBufferInfo = &buffer->BufferInfo;
			WriteDescriptorSet[i + Offset].dstArrayElement = 0;
			WriteDescriptorSet[i + Offset].dstBinding = static_cast<uint32_t>(i);*/
		}
		Offset += CountBuffers;

		vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(Count), WriteDescriptorSet, 0, NULL);
	}
}

VKRenderRootSignature::~VKRenderRootSignature()
{
	vkFreeDescriptorSets(Factory->Device, DescriptorPool,1,&DescriptorSet);
	vkDestroyDescriptorPool(Factory->Device, DescriptorPool, 0);
	vkDestroyPipelineLayout(Factory->Device, PipelineLayout, 0);
	vkDestroyDescriptorSetLayout(Factory->Device,DescriptorSetLayout,0);
}

void VKRenderRootSignature::Set(VkCommandBuffer CommandLine)
{
	vkCmdBindDescriptorSets(CommandLine, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1,&DescriptorSet ,0, NULL);
}
