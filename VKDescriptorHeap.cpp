#include "VKPCH.h"
size_t DescriptorHeapCounter = 0;
VKDescriptorHeap::VKDescriptorHeap(const BearDescriptorHeapDescription& Description)
{
	DescriptorHeapCounter++;
	CountBuffers = 0;
	CountSamplers = 0;
	CountSRVs = 0;

	BEAR_ASSERT(!Description.RootSignature.empty());

	const VKRootSignature* RootSig = static_cast<const VKRootSignature*>(Description.RootSignature.get());
	{


		CountBuffers = static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountBuffers;
		CountSRVs = static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountSRVs;
		CountSamplers = static_cast<const VKRootSignature*>(Description.RootSignature.get())->CountSamplers;


		memcpy(SlotBuffers, static_cast<const VKRootSignature*>(Description.RootSignature.get())->SlotBuffers, 16 * sizeof(size_t));
		memcpy(SlotSRVs, static_cast<const VKRootSignature*>(Description.RootSignature.get())->SlotSRVs, 16 * sizeof(size_t));
		memcpy(SlotSamplers, static_cast<const VKRootSignature*>(Description.RootSignature.get())->SlotSamplers, 16 * sizeof(size_t));
	}

	{
		VkDescriptorPoolSize PoolSizes[64];
		size_t Offset = 0;
		for (size_t i = 0; i < CountBuffers; i++)
		{
			PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			PoolSizes[i + Offset].descriptorCount = 1;
		}
		Offset += CountBuffers;

		for (size_t i = 0; i < CountSRVs; i++)
		{
			switch (RootSig->Description.SRVResources[RootSig->SlotSRVs[i]].DescriptorType)
			{
			case BearDescriptorType::DT_Buffer:
				PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case BearDescriptorType::DT_Image:
				PoolSizes[i + Offset].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;
			default:
				BEAR_CHECK(0);
				break;
			}
			PoolSizes[i + Offset].descriptorCount = 1;
		}
		Offset += CountSRVs;
		for (size_t i = 0; i < CountSamplers; i++)
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

void VKDescriptorHeap::SetUniformBuffer(size_t slot, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> UniformBuffer, size_t offset)
{
	if (UniformBuffer.empty())return;
	BEAR_CHECK(slot < 16);
	/*slot = SlotBuffers[slot];
	BEAR_CHECK(slot < CountBuffers);*/
	if (UniformBuffers[slot] == UniformBuffer)
	{
		if (UniformBufferOffsets[slot] == offset)
		{
			return;
		}

	}
	UniformBuffers[slot] = UniformBuffer;
	UniformBufferOffsets[slot] = offset;
	VkWriteDescriptorSet WriteDescriptorSet = {};
	auto* buffer = static_cast<const VKUniformBuffer*>(UniformBuffer.get());
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorCount = 1;
	WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	VkDescriptorBufferInfo BufferInfo;
	BufferInfo.buffer = buffer->Buffer;
	BufferInfo.range = buffer->Stride;
	BufferInfo.offset = buffer->Stride* offset;

	WriteDescriptorSet.pBufferInfo = &BufferInfo;
	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.dstBinding = static_cast<uint32_t>( slot);;
	vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(1), &WriteDescriptorSet, 0, NULL);
}

void VKDescriptorHeap::SetShaderResource(size_t slot, BearFactoryPointer<BearRHI::BearRHIShaderResource> ShaderResource,size_t offset)
{
	if (ShaderResource.empty())return;
	BEAR_CHECK(slot < 16);
	/*slot = SlotSRVs[slot];
	BEAR_CHECK(slot < CountSRVs);*/
	if (ShaderResources[slot] == ShaderResource)
	{
		if (ShaderResourcesOffsets[slot] == offset)return;
	}
	ShaderResourcesOffsets[slot] = offset;
	ShaderResources[slot] = ShaderResource;
	VkWriteDescriptorSet WriteDescriptorSet = {};
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorCount = 1;
	VKShaderResource* P = reinterpret_cast<VKShaderResource*>(ShaderResource.get()->QueryInterface(VKQ_ShaderResource));
	BEAR_ASSERT(P);
	P->SetAsSRV(&WriteDescriptorSet, offset);
	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.dstBinding = static_cast<uint32_t>(16 + slot);
	vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(1), &WriteDescriptorSet, 0, NULL);
}

void VKDescriptorHeap::SetSampler(size_t slot, BearFactoryPointer<BearRHI::BearRHISampler> Sampler)
{
	if (Sampler.empty())return;
	BEAR_CHECK(slot < 16);
	//slot = SlotSamplers[slot];
//	BEAR_CHECK(slot < CountSamplers);
	if (Samplers[slot] == Sampler)return;

	Samplers[slot] = Sampler;
	VkWriteDescriptorSet WriteDescriptorSet = {};
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorCount = 1;
	auto* buffer = static_cast<const VKSamplerState*>(Sampler.get());

	WriteDescriptorSet.dstArrayElement = 0;
	WriteDescriptorSet.dstBinding = static_cast<uint32_t>(32+ slot);
	WriteDescriptorSet.pImageInfo = &buffer->ImageInfo;
	WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	vkUpdateDescriptorSets(Factory->Device, static_cast<uint32_t>(1), &WriteDescriptorSet, 0, NULL);
}
