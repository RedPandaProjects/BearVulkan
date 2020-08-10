#include "VKPCH.h"
size_t ContextCounter = 0;
extern bool GDebugRender;
extern PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
extern PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;

VKContext::VKContext()
{
	PipelineType = PT_Graphics;
	m_UseRenderPass = false;
	ContextCounter++;
	VkCommandPoolCreateInfo cmd_pool_info = {};
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_info.pNext = NULL;
	cmd_pool_info.queueFamilyIndex = Factory->QueueFamilyIndex;
	cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	V_CHK(vkCreateCommandPool(Factory->Device, &cmd_pool_info, NULL, &CommandPool));


	VkCommandBufferAllocateInfo cmd = {};
	cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd.pNext = NULL;
	cmd.commandPool = CommandPool;
	cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd.commandBufferCount = 1;

	V_CHK(vkAllocateCommandBuffers(Factory->Device, &cmd, &CommandBuffer));
//	vkFreeCommandBuffers
	VkFenceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	V_CHK(vkCreateFence(Factory->Device, &info, nullptr, &Fence));
	V_CHK(vkWaitForFences(Factory->Device, 1, &Fence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &Fence));
	VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
	imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAcquiredSemaphoreCreateInfo.pNext = NULL;
	imageAcquiredSemaphoreCreateInfo.flags = 0;

	V_CHK(vkCreateSemaphore(Factory->Device, &imageAcquiredSemaphoreCreateInfo, NULL, &SemaphoreWait));


}
VKContext::~VKContext()
{
	vkDestroySemaphore(Factory->Device, SemaphoreWait, 0);
	vkDestroyFence(Factory->Device,Fence,0);
	VkCommandBuffer cmd_bufs[1] = { CommandBuffer };
	vkFreeCommandBuffers(Factory->Device, CommandPool, 1, cmd_bufs);
	vkDestroyCommandPool(Factory->Device, CommandPool, NULL);
	ContextCounter--;
}

void VKContext::Reset()
{
	m_UseRenderPass = false;
	VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo;
	{
		CommandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		CommandBufferInheritanceInfo.pNext = nullptr;
		CommandBufferInheritanceInfo.renderPass = 0;
		CommandBufferInheritanceInfo.subpass = 0;
		CommandBufferInheritanceInfo.framebuffer = 0;
		CommandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		CommandBufferInheritanceInfo.queryFlags = 0;
		CommandBufferInheritanceInfo.pipelineStatistics = 0;
	}
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	{
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.pNext = nullptr;
		CommandBufferBeginInfo.flags = 0;
		CommandBufferBeginInfo.pInheritanceInfo = &CommandBufferInheritanceInfo;
	}
	V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));
}
void VKContext::Wait()
{
	V_CHK(vkWaitForFences(Factory->Device, 1, &Fence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &Fence));
}
void VKContext::Flush(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport, bool wait)
{
	static VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	if (m_UseRenderPass)vkCmdEndRenderPass(CommandBuffer);
	V_CHK(vkEndCommandBuffer(CommandBuffer));
	auto viewport = static_cast<VKViewport*>(Viewport.get());
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = 0;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = &viewport->Semaphore;// &buf.acquire_semaphore;
	submit_info.pWaitDstStageMask = &stage;
	submit_info.pCommandBuffers = &CommandBuffer;
	submit_info.commandBufferCount = 1;
	V_CHK(vkQueueSubmit(Factory->Queue, 1, &submit_info, Fence));
	viewport->Swap();
	if (wait)Wait();
}
void VKContext::Flush(bool wait)
{

	static VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	if(m_UseRenderPass)vkCmdEndRenderPass(CommandBuffer);
	V_CHK(vkEndCommandBuffer(CommandBuffer));
	/**/
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = 0;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = 0;// &buf.acquire_semaphore;
	submit_info.pWaitDstStageMask = &stage;
	submit_info.pCommandBuffers = &CommandBuffer;
	submit_info.commandBufferCount = 1;
	V_CHK(vkQueueSubmit(Factory->Queue, 1, &submit_info, Fence));

	if (wait)Wait();
}

void VKContext::ClearState()
{
	if(m_UseRenderPass)
	vkCmdEndRenderPass(CommandBuffer);
	m_UseRenderPass = false;
}

void VKContext::BeginEvent(char const* name, BearColor color)
{
	if (GDebugRender)
	{
		VkDebugUtilsLabelEXT Info = {};
		Info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		Info.pLabelName = name;
		memcpy(Info.color, color.R32G32B32A32, sizeof(float) * 4);
		CmdBeginDebugUtilsLabelEXT(CommandBuffer, &Info);
	}

}
void VKContext::EndEvent()
{
	if (GDebugRender)
	{
		CmdEndDebugUtilsLabelEXT(CommandBuffer);
	}
}



void VKContext::SetViewportAsFrameBuffer(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport)
{
	if (m_UseRenderPass)vkCmdEndRenderPass(CommandBuffer);
	VkRenderPassBeginInfo RenderPassBeginInfo = static_cast<VKViewport*>(Viewport.get())->GetRenderPass();
	vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_UseRenderPass = true;
}
void VKContext::SetFrameBuffer(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> FrameBuffer)
{
	if (m_UseRenderPass)vkCmdEndRenderPass(CommandBuffer);
	VkRenderPassBeginInfo RenderPassBeginInfo = static_cast<VKFrameBuffer*>(FrameBuffer.get())->GetRenderPass();
	vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_UseRenderPass = true;
}
void VKContext::SetPipeline(BearFactoryPointer<BearRHI::BearRHIPipeline> Pipeline)
{
	VKPipeline* P = reinterpret_cast<VKPipeline*>(Pipeline.get()->QueryInterface(VKQ_Pipeline));
	BEAR_CHECK(P);
	P->Set(CommandBuffer);
	PipelineType = P->GetType();
}
void VKContext::SetDescriptorHeap(BearFactoryPointer<BearRHI::BearRHIDescriptorHeap> DescriptorHeap)
{
	switch (PipelineType)
	{
	case PT_RayTracing:
		static_cast<VKDescriptorHeap*>(DescriptorHeap.get())->SetRayTracing(CommandBuffer);
		break;
	default:
		static_cast<VKDescriptorHeap*>(DescriptorHeap.get())->SetGraphics(CommandBuffer);
		break;
	}

}
void VKContext::SetVertexBuffer(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> buffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &static_cast<VKVertexBuffer*>(buffer.get())->Buffer, &offset);
}
void VKContext::SetIndexBuffer(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> buffer)
{
	vkCmdBindIndexBuffer(CommandBuffer, static_cast<VKIndexBuffer*>(buffer.get())->Buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);
}
void VKContext::SetStencilRef(uint32 ref)
{
	vkCmdSetStencilReference(CommandBuffer, VK_STENCIL_FRONT_AND_BACK, ref);
}
void VKContext::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	Viewport.x = x;
	Viewport.y = height - y;
	Viewport.width = width;
	Viewport.height = -height;
	Viewport.maxDepth = maxDepth;
	Viewport.minDepth = minDepth;
	vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
}
void VKContext::SetScissor(bool Enable, float x, float y, float x1, float y1)
{
	VkRect2D Scissor;

	if (Enable)
	{
		Scissor.offset.x = static_cast<int>(x);
		Scissor.offset.y = static_cast<int>(y);
		Scissor.extent.width = static_cast<uint32>(x1-x);
		Scissor.extent.height = static_cast<uint32>(y1-y);
	}
	else
	{
		Scissor.offset.x = static_cast<int>(Viewport.x);
		Scissor.offset.y = static_cast<int>(Viewport.height) + static_cast<int>(Viewport.y);
		Scissor.extent.width = static_cast<uint32>(Viewport.width);
		Scissor.extent.height = static_cast<uint32>(abs(Viewport.height));
	}

	vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
}
void VKContext::Draw(size_t count, size_t offset)
{
	vkCmdDraw(CommandBuffer, static_cast<uint32>(count), 1, static_cast<uint32>(offset), 0);
}
void VKContext::DrawIndex(size_t count, size_t  offset_index, size_t offset_vertex)
{
	vkCmdDrawIndexed(CommandBuffer, static_cast<uint32>(count), 1, static_cast<uint32>(offset_index), static_cast<uint32>(offset_vertex), 0);
}

void VKContext::DispatchMesh(size_t CountX, size_t CountY, size_t CountZ)
{
}

void VKContext::DispatchRays(const BearDispatchRaysDescription& Description)
{
#ifdef RTX
	auto* RayGenerationTable = static_cast<const VKRayTracingShaderTable*>(Description.RayGeneration.get());
	auto* MissTable = static_cast<const VKRayTracingShaderTable*>(Description.Miss.get());
	auto* HitGroupTable = static_cast<const VKRayTracingShaderTable*>(Description.HitGroup.get());
	vkCmdTraceRaysNV(CommandBuffer, RayGenerationTable->Buffer, 0, MissTable->Buffer, 0, MissTable->Size, HitGroupTable->Buffer, 0, HitGroupTable->Size, VK_NULL_HANDLE, 0, 0, Description.Width, Description.Height, Description.Depth);
#endif
}



void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Src)
{
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKIndexBuffer*>(Dst.get())->Buffer == 0)return;
	if (static_cast<VKIndexBuffer*>(Src.get())->Buffer == 0)return;
	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	{
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	}
	V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	CopyBuffer(CommandBuffer, static_cast<VKIndexBuffer*>(Src.get())->Buffer, static_cast<VKIndexBuffer*>(Dst.get())->Buffer, static_cast<VKIndexBuffer*>(Dst.get())->Size);
}
void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Src)
{
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKVertexBuffer*>(Dst.get())->Buffer == 0)return;

	if (static_cast<VKVertexBuffer*>(Src.get())->Buffer == 0)return;
	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	{
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	}
	V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));
	CopyBuffer(CommandBuffer, static_cast<VKVertexBuffer*>(Src.get())->Buffer, static_cast<VKVertexBuffer*>(Dst.get())->Buffer, static_cast<VKVertexBuffer*>(Dst.get())->Size);
}
void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHITexture2D> Dst, BearFactoryPointer<BearRHI::BearRHITexture2D> Src)
{
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKTexture2D*>(Dst.get())->Image == 0)return;

	if (static_cast<VKTexture2D*>(Src.get())->Image == 0)return;
	auto dst = static_cast<VKTexture2D*>(Dst.get());
	auto src = static_cast<VKTexture2D*>(Src.get());


	if (src->ImageInfo.extent.width != dst->ImageInfo.extent.width)return;
	if (src->ImageInfo.extent.height != dst->ImageInfo.extent.height)return;;
	if (src->ImageInfo.arrayLayers != dst->ImageInfo.arrayLayers)return;;
	if (dst->ImageInfo.mipLevels != 1)
		if (src->ImageInfo.mipLevels != dst->ImageInfo.mipLevels)return;

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	{
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	}
	V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));
	{

		TransitionImageLayout(CommandBuffer, dst->Image, dst->ImageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dst->ImageInfo.mipLevels, dst->ImageInfo.arrayLayers, 0);
		TransitionImageLayout(CommandBuffer, src->Image, src->ImageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src->ImageInfo.mipLevels, src->ImageInfo.arrayLayers, 0);

		for (uint32_t i = 0; i < dst->ImageInfo.mipLevels; i++)
		{
			VkImageCopy imgc = {};
			imgc.extent.width = dst->ImageInfo.extent.width;
			imgc.extent.height = dst->ImageInfo.extent.height;
			imgc.extent.depth = dst->ImageInfo.arrayLayers;
			imgc.dstSubresource.mipLevel = i;
			imgc.srcSubresource.mipLevel = i;
			vkCmdCopyImage(CommandBuffer, src->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imgc);
		}
		TransitionImageLayout(CommandBuffer, dst->Image, dst->ImageInfo.format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dst->ImageInfo.initialLayout, dst->ImageInfo.mipLevels, dst->ImageInfo.arrayLayers, 0);
		TransitionImageLayout(CommandBuffer, src->Image, src->ImageInfo.format, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src->ImageInfo.initialLayout, src->ImageInfo.mipLevels, src->ImageInfo.arrayLayers, 0);
	}

}
void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Src)
{
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKUniformBuffer*>(Dst.get())->Buffer == 0)return;

	if (static_cast<VKUniformBuffer*>(Src.get())->Buffer == 0)return;
	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	{
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	}
	V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	CopyBuffer(CommandBuffer, static_cast<VKUniformBuffer*>(Src.get())->Buffer, static_cast<VKUniformBuffer*>(Dst.get())->Buffer, static_cast<VKUniformBuffer*>(Dst.get())->Count* static_cast<VKUniformBuffer*>(Dst.get())->Stride);

}

void VKContext::Lock(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport)
{
}

void VKContext::Unlock(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport)
{
}

void VKContext::Lock(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> Framebuffer)
{
	BEAR_CHECK(!Framebuffer.empty());
	static_cast<VKFrameBuffer*>(Framebuffer.get())->Lock(CommandBuffer);
}

void VKContext::Unlock(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> Framebuffer)
{
	BEAR_CHECK(!Framebuffer.empty());
	static_cast<VKFrameBuffer*>(Framebuffer.get())->Unlock(CommandBuffer);
}
void VKContext::Lock(BearFactoryPointer<BearRHI::BearRHIUnorderedAccess> UnorderedAccess)
{
	BEAR_CHECK(!UnorderedAccess.empty());
	VKUnorderedAccess* UAV = reinterpret_cast<VKUnorderedAccess*>(UnorderedAccess.get()->QueryInterface(VKQ_UnorderedAccess));
	UAV->LockUAV(CommandBuffer);
}
void VKContext::Unlock(BearFactoryPointer<BearRHI::BearRHIUnorderedAccess> UnorderedAccess)
{
	BEAR_CHECK(!UnorderedAccess.empty());
	VKUnorderedAccess* UAV = reinterpret_cast<VKUnorderedAccess*>(UnorderedAccess.get()->QueryInterface(VKQ_UnorderedAccess));
	UAV->UnlockUAV(CommandBuffer);
}