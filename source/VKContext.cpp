#include "VKPCH.h"
bsize ContextCounter = 0;
VKContext::VKContext():m_Status(0)
{
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
	Viewport.width = 1;
	Viewport.height = 1;
	Viewport.x = Viewport.y = 0;
	Viewport.maxDepth = 1;
	Viewport.minDepth = 0;
	Scissor.offset.x = 0;
	Scissor.offset.y = 0;
	Scissor.extent.width = 1;
	Scissor.extent.width = 1;
	ScissorEnable = false;
	m_use_renderpass = false;
}

VKContext::~VKContext()
{
	PreDestroy();
	vkDestroySemaphore(Factory->Device, SemaphoreWait, 0);
	vkDestroyFence(Factory->Device,Fence,0);
	VkCommandBuffer cmd_bufs[1] = { CommandBuffer };
	vkFreeCommandBuffers(Factory->Device, CommandPool, 1, cmd_bufs);
	vkDestroyCommandPool(Factory->Device, CommandPool, NULL);
	ContextCounter--;
}

void VKContext::Wait()
{
	if (m_Status != 2)
		return;
	
	V_CHK(vkWaitForFences(Factory->Device, 1, &Fence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &Fence));

	if (!m_viewport.empty())static_cast<VKViewport*>(m_viewport.get())->Swap();
	m_Status = 0;

}

void VKContext::Flush(bool wait)
{
	static VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;


	if (m_Status != 1)
		return;
	if(m_use_renderpass)
	vkCmdEndRenderPass(CommandBuffer);
	if (!m_frame_buffer.empty())
	{
		auto frame_buffer = static_cast<VKFrameBuffer*>(m_frame_buffer.get());
		frame_buffer->ToTexture(CommandBuffer);
	}
	m_use_renderpass = false;
	V_CHK(vkEndCommandBuffer(CommandBuffer));

	if (m_viewport.empty()==false)
	{
		auto viewport = static_cast<VKViewport*>(m_viewport.get());
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 0;
		submit_info.pWaitSemaphores = 0;
		submit_info.signalSemaphoreCount =0;
		submit_info.pSignalSemaphores = &viewport->Semaphore;// &buf.acquire_semaphore;
		submit_info.pWaitDstStageMask = &stage;
		submit_info.pCommandBuffers = &CommandBuffer;
		submit_info.commandBufferCount = 1;
		V_CHK(vkQueueSubmit(Factory->Queue, 1, &submit_info, Fence));
	}
	else
	{
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
	}
	m_Status = 2;
	if (wait)Wait();

}



void VKContext::AttachViewportAsFrameBuffer(BearFactoryPointer<BearRHI::BearRHIViewport> Viewport_)
{
	DetachFrameBuffer();
	m_viewport = Viewport_;

}

void VKContext::AttachFrameBuffer(BearFactoryPointer<BearRHI::BearRHIFrameBuffer> FrameBuffer)
{
	DetachFrameBuffer();
	m_frame_buffer = FrameBuffer;
}



void VKContext::DetachFrameBuffer()
{
	PreDestroy();
	m_frame_buffer.clear();
	m_viewport.clear();
}

void VKContext::ClearFrameBuffer()
{
	if (UpdateStatus() != 0)return;
	m_Status = 1;
	

	VkFramebuffer FreameBuffer = 0;
	VkRenderPassBeginInfo RenderPassBeginInfo;

	if (m_viewport.get())
	{
		VKViewport* SwapChain = static_cast<VKViewport*>(m_viewport.get());
		FreameBuffer = SwapChain->Framebuffers[SwapChain->FrameIndex];
		RenderPassBeginInfo = SwapChain->GetRenderPass();
	}
	else if (m_frame_buffer.get())
	{
		auto frame_buffer = static_cast<VKFrameBuffer*>(m_frame_buffer.get());
		FreameBuffer = frame_buffer->FrameBuffer;
		RenderPassBeginInfo = frame_buffer->GetRenderPass();
	}
	else
	{
		return;
	}
	

	VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo;
	{
		CommandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		CommandBufferInheritanceInfo.pNext = nullptr;
		CommandBufferInheritanceInfo.renderPass = 0;
		CommandBufferInheritanceInfo.subpass = 0;
		CommandBufferInheritanceInfo.framebuffer = FreameBuffer;
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

	if (m_frame_buffer.get())
	{
		auto frame_buffer = static_cast<VKFrameBuffer*>(m_frame_buffer.get());
		frame_buffer->ToRT(CommandBuffer);
	}

	vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_use_renderpass = true;
	{
		vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
		if (!ScissorEnable)
		{
			Scissor.offset.x = static_cast<int32>(Viewport.x);
			Scissor.offset.y = static_cast<int32>(Viewport.height)  +static_cast<int32>(Viewport.y);
			Scissor.extent.width = static_cast<uint32>(Viewport.width);
			Scissor.extent.height = static_cast<uint32>(abs(Viewport.height));

		}
		vkCmdSetScissor(CommandBuffer,0,1,&Scissor);
	}

}
void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIIndexBuffer> Src)
{
	if (m_Status == 2)return;
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKIndexBuffer*>(Dst.get())->Buffer == 0)return;
	if (static_cast<VKIndexBuffer*>(Src.get())->Buffer == 0)return;
	if (m_Status == 0)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		{
			CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	}

	CopyBuffer(CommandBuffer, static_cast<VKIndexBuffer*>(Src.get())->Buffer, static_cast<VKIndexBuffer*>(Dst.get())->Buffer, static_cast<VKIndexBuffer*>(Dst.get())->Size);
	m_Status = 1;
}
void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIVertexBuffer> Src)
{
	if (m_Status == 2)return;
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKVertexBuffer*>(Dst.get())->Buffer == 0)return;

	if (static_cast<VKVertexBuffer*>(Src.get())->Buffer == 0)return;
	if (m_Status == 0)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		{
			CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	}
	CopyBuffer(CommandBuffer, static_cast<VKVertexBuffer*>(Src.get())->Buffer, static_cast<VKVertexBuffer*>(Dst.get())->Buffer, static_cast<VKVertexBuffer*>(Dst.get())->Size);
	m_Status = 1;
}
 void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHITexture2D> Dst, BearFactoryPointer<BearRHI::BearRHITexture2D> Src)
{
	if (m_Status == 2)return;
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKTexture2D*>(Dst.get())->Image == 0)return;

	if (static_cast<VKTexture2D*>(Src.get())->Image == 0)return;
	auto dst = static_cast<VKTexture2D*>(Dst.get());
	auto src = static_cast<VKTexture2D*>(Src.get());


	if (src->ImageInfo.extent.width != dst->ImageInfo.extent.width)return;
	if (src->ImageInfo.extent.height != dst->ImageInfo.extent.height)return;;
	if (src->ImageInfo.extent.depth != dst->ImageInfo.extent.depth)return;;
	if (dst->ImageInfo.mipLevels!=1)
	 if (src->ImageInfo.mipLevels != dst->ImageInfo.mipLevels)return;
	if (m_Status == 0)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		{
			CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	}
	{

		TransitionImageLayout(CommandBuffer,dst->Image, dst->ImageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dst->ImageInfo.mipLevels, dst->ImageInfo.extent.depth);
		TransitionImageLayout(CommandBuffer, src->Image, src->ImageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src->ImageInfo.mipLevels, src->ImageInfo.extent.depth);

	

		
	
		for (uint32_t i = 0; i < dst->ImageInfo.mipLevels; i++)
		{
			ImageCopyStack.emplace_back(VkImageCopy());
			bear_fill(ImageCopyStack.back());
			ImageCopyStack.back().extent.width = dst->ImageInfo.extent.width;
			ImageCopyStack.back().extent.height = dst->ImageInfo.extent.height;
			ImageCopyStack.back().extent.depth = dst->ImageInfo.extent.depth;
			ImageCopyStack.back().dstSubresource.mipLevel = i;
			ImageCopyStack.back().srcSubresource.mipLevel = i;
			vkCmdCopyImage(CommandBuffer, src->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1, &ImageCopyStack.back());
		}
		TransitionImageLayout(CommandBuffer, dst->Image, dst->ImageInfo.format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dst->ImageInfo.initialLayout, dst->ImageInfo.mipLevels, dst->ImageInfo.extent.depth);
		TransitionImageLayout(CommandBuffer, src->Image, src->ImageInfo.format, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src->ImageInfo.initialLayout, src->ImageInfo.mipLevels, src->ImageInfo.extent.depth);
	}

}
void VKContext::Copy(BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Dst, BearFactoryPointer<BearRHI::BearRHIUniformBuffer> Src)
{
	if (m_Status == 2)return;
	if (Dst.empty() || Src.empty())return;
	if (static_cast<VKUniformBuffer*>(Dst.get())->Buffer == 0)return;

	if (static_cast<VKUniformBuffer*>(Src.get())->Buffer == 0)return;
	if (m_Status == 0)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		{
			CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		V_CHK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	}
	CopyBuffer(CommandBuffer, static_cast<VKUniformBuffer*>(Src.get())->Buffer, static_cast<VKUniformBuffer*>(Dst.get())->Buffer, static_cast<VKUniformBuffer*>(Dst.get())->Size);
	m_Status = 1;
}
void VKContext::SetDescriptorHeap(BearFactoryPointer<BearRHI::BearRHIDescriptorHeap> DescriptorHeap)
{
	if (m_Status != 1 || DescriptorHeap.get() == 0)return;

	static_cast<VKDescriptorHeap*>(DescriptorHeap.get())->Set(CommandBuffer);
}
void VKContext::SetPipeline(BearFactoryPointer<BearRHI::BearRHIPipeline> Pipeline)
{
	if (m_Status != 1 || Pipeline.get() == 0)return;

	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(Pipeline.get())->Pipeline);
	

}
void VKContext::SetVertexBuffer(BearFactoryPointer<BearRHI::BearRHIVertexBuffer> buffer)
{
	if (m_Status != 1 || buffer.get() == 0)return;
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &static_cast<VKVertexBuffer*>(buffer.get())->Buffer, &offset);

}
void VKContext::SetIndexBuffer(BearFactoryPointer<BearRHI::BearRHIIndexBuffer> buffer)
{
	if (m_Status != 1 || buffer.get() == 0)return;
	vkCmdBindIndexBuffer(CommandBuffer, static_cast<VKIndexBuffer*>(buffer.get())->Buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);

}
void VKContext::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	Viewport.x = x;
	Viewport.y = height - y;
	Viewport.width = width;
	Viewport.height = -height;
	Viewport.maxDepth = maxDepth;
	Viewport.minDepth = minDepth;
	if (m_Status == 1)
	{

		vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);


		vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
	}
}
void VKContext::SetScissor(bool Enable, float x, float y, float x1, float y1)
{
	ScissorEnable = Enable;

	if (ScissorEnable)
	{
		Scissor.offset.x = static_cast<int32>(x);
		Scissor.offset.y = static_cast<int32>(y);
		Scissor.extent.width = static_cast<uint32>(x1-x);
		Scissor.extent.height = static_cast<uint32>(y1-y);
	}
	else
	{
		Scissor.offset.x = static_cast<int32>(Viewport.x);
		Scissor.offset.y = static_cast<int32>(Viewport.height) + static_cast<int32>(Viewport.y);
		Scissor.extent.width = static_cast<uint32>(Viewport.width);
		Scissor.extent.height = static_cast<uint32>(abs(Viewport.height));
	}

	if (m_Status != 1)return;
	vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
}
void VKContext::Draw(bsize count, bsize offset)
{
	if (m_Status != 1)return;
	vkCmdDraw(CommandBuffer, static_cast<uint32>(count), 1, static_cast<uint32>(offset), 0);

}
void VKContext::DrawIndex(bsize count, bsize  offset_index, bsize offset_vertex)
{
	if (m_Status != 1)return;
	vkCmdDrawIndexed(CommandBuffer, static_cast<uint32>(count), 1, static_cast<uint32>(offset_index), static_cast<uint32>(offset_vertex), 0);

}
void VKContext::PreDestroy()
{
	if (m_Status == 1)Flush(true);
	if (m_Status == 2)Wait();
}

int VKContext::UpdateStatus()
{
	if (m_Status == 2)Wait();
	return m_Status;
}
