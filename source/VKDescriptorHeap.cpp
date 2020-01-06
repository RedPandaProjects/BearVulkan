#include "VKPCH.h"

VKDescriptorHeap::VKDescriptorHeap(const BearDescriptorHeapDescription& Description)
{
	BEAR_RASSERT(!Description.RootSignature.empty());
	for (; CountBuffers < 16 && !Description.UniformBuffers[CountBuffers].empty(); CountBuffers++);
	BEAR_ASSERT(CountBuffers == static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountBuffers);
	{
		RootSignature = Description.RootSignature;
		RootSignaturePointer = static_cast<VKRootSignature*>(RootSignature.get());
	}

	{
		VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo[1];
		DescriptorSetAllocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		DescriptorSetAllocateInfo[0].pNext = NULL;
		DescriptorSetAllocateInfo[0].descriptorPool = RootSignaturePointer->DescriptorPool;
		DescriptorSetAllocateInfo[0].descriptorSetCount = 1;
		DescriptorSetAllocateInfo[0].pSetLayouts = &(RootSignaturePointer->DescriptorSetLayout);

		V_CHK(vkAllocateDescriptorSets(Factory->Device, DescriptorSetAllocateInfo, &DescriptorSet));


		VkWriteDescriptorSet WriteDescriptorSet[64];
		bsize Offset = 0;
		for (bsize i = 0; i < CountBuffers; i++)
		{
			auto *buffer = static_cast<const VKUniformBuffer*>(Description.UniformBuffers[i].get());
			WriteDescriptorSet[i + Offset] = {};
			WriteDescriptorSet[i + Offset].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			WriteDescriptorSet[i + Offset].pNext = NULL;
			WriteDescriptorSet[i + Offset].dstSet = DescriptorSet;
			WriteDescriptorSet[i + Offset].descriptorCount = 1;
			WriteDescriptorSet[i + Offset].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			WriteDescriptorSet[i + Offset].pBufferInfo = &buffer->BufferInfo;
			WriteDescriptorSet[i + Offset].dstArrayElement = 0;
			WriteDescriptorSet[i + Offset].dstBinding = static_cast<uint32_t>(i);
		}
		Offset += CountBuffers;

		vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(Offset), WriteDescriptorSet, 0, NULL);
	}
}

VKDescriptorHeap::~VKDescriptorHeap()
{
	vkFreeDescriptorSets(Factory->Device, RootSignaturePointer-> DescriptorPool, 1, &DescriptorSet);
}

void VKDescriptorHeap::Set(VkCommandBuffer CommandLine)
{
	vkCmdBindDescriptorSets(CommandLine, VK_PIPELINE_BIND_POINT_GRAPHICS, RootSignaturePointer->PipelineLayout, 0, 1, &DescriptorSet, 0, NULL);
}
