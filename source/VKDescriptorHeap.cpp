#include "VKPCH.h"
bsize DescriptorHeapCounter = 0;
VKDescriptorHeap::VKDescriptorHeap(const BearDescriptorHeapDescription& Description)
{
	DescriptorHeapCounter++;
	BEAR_RASSERT(!Description.RootSignature.empty());
	CountSamplers = 0;
	CountSRVs = 0;
	CountBuffers = 0;

	
	CountBuffers = static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountBuffers;
	CountSRVs = static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountSRVs;
	CountSamplers = static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountSamplers;

	{
		VkDescriptorPoolSize PoolSizes[64];
		bsize Offset = 0;
		for (bsize i = 0; i < CountBuffers; i++)
		{
			PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			PoolSizes[i + Offset].descriptorCount = 1;
		}
		Offset += CountBuffers;

		for (bsize i = 0; i < CountSRVs; i++)
		{
			PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			PoolSizes[i + Offset].descriptorCount = 1;
		}
		Offset += CountSRVs;
		for (bsize i = 0; i < CountSamplers; i++)
		{
			PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_SAMPLER;
			PoolSizes[i + Offset].descriptorCount = 1;
		}

		VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
		DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.pNext = NULL;
		DescriptorPoolCreateInfo.maxSets = 1;
		DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(CountBuffers+ CountSRVs+ CountSamplers);;;
		DescriptorPoolCreateInfo.pPoolSizes = PoolSizes;

		V_CHK(vkCreateDescriptorPool(Factory->Device, &DescriptorPoolCreateInfo, NULL, &DescriptorPool));
	}

	{
		RootSignature = Description.RootSignature;
		RootSignaturePointer = static_cast<VKRootSignature*>(RootSignature.get());
	}
	{
		
			VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo[1];
			DescriptorSetAllocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			DescriptorSetAllocateInfo[0].pNext = NULL;
			DescriptorSetAllocateInfo[0].descriptorPool = DescriptorPool;
			DescriptorSetAllocateInfo[0].descriptorSetCount = 1;
			DescriptorSetAllocateInfo[0].pSetLayouts = &(RootSignaturePointer->DescriptorSetLayout);

			V_CHK(vkAllocateDescriptorSets(Factory->Device, DescriptorSetAllocateInfo, &DescriptorSet));
	}
	{
	

	//	BearDebug::DebugBreak();
		
	}
}

VKDescriptorHeap::~VKDescriptorHeap()
{
	DescriptorHeapCounter--;

	vkDestroyDescriptorPool(Factory->Device, DescriptorPool, 0);
}

void VKDescriptorHeap::Set(VkCommandBuffer CommandLine)
{
	vkCmdBindDescriptorSets(CommandLine, VK_PIPELINE_BIND_POINT_GRAPHICS, RootSignaturePointer->PipelineLayout, 0, 1, &DescriptorSet, 0, NULL);
}

void VKDescriptorHeap::SetUniformBuffer(bsize slot, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> UniformBuffer)
{
	if (UniformBuffer.empty())return;
	BEAR_RASSERT(slot < CountBuffers);
	if (UniformBuffers[slot] == UniformBuffer)return;

	UniformBuffers[slot] = UniformBuffer;
	VkWriteDescriptorSet WriteDescriptorSet = {};
	auto* buffer = static_cast<const VKUniformBuffer*>(UniformBuffer.get());
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorCount = 1;
	WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	WriteDescriptorSet.pBufferInfo = &buffer->BufferInfo;
	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.dstBinding = static_cast<uint32_t>(CountSRVs+ CountSamplers+ slot);;
	vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(1), &WriteDescriptorSet, 0, NULL);
}

void VKDescriptorHeap::SetShaderResource(bsize slot, BearFactoryPointer<BearRHI::BearRHIShaderResource> ShaderResource)
{
	if (ShaderResource.empty())return;
	BEAR_RASSERT(slot < CountSRVs);
	if (ShaderResources[slot] == ShaderResource)return;

	ShaderResources[slot] = ShaderResource;
	VkWriteDescriptorSet WriteDescriptorSet = {};
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorCount = 1;
	auto* buffer = const_cast<VKShaderResource*>(dynamic_cast<const VKShaderResource*>(ShaderResource.get()));
	buffer->Set(&WriteDescriptorSet);
	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.dstBinding = static_cast<uint32_t>(slot);
	vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(1), &WriteDescriptorSet, 0, NULL);
}

void VKDescriptorHeap::SetSampler(bsize slot, BearFactoryPointer<BearRHI::BearRHISampler> Sampler)
{
	if (Sampler.empty())return;
	BEAR_RASSERT(slot < CountSamplers);
	if (Samplers[slot] == Sampler)return;

	Samplers[slot] = Sampler;
	VkWriteDescriptorSet WriteDescriptorSet = {};
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorCount = 1;
	auto* buffer = static_cast<const VKSamplerState*>(Sampler.get());

	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.dstBinding = static_cast<uint32_t>(CountSRVs+ slot);
	WriteDescriptorSet.pImageInfo = &buffer->ImageInfo;
	WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(1), &WriteDescriptorSet, 0, NULL);
}
