#include "vulkanPCH.h"

VKRenderContext::VKRenderContext():m_Status(0)
{
	VkCommandPoolCreateInfo cmd_pool_info = {};
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_info.pNext = NULL;
	cmd_pool_info.queueFamilyIndex = Factory->QueueFamilyIndex;
	cmd_pool_info.flags = 0;

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

	VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
	imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAcquiredSemaphoreCreateInfo.pNext = NULL;
	imageAcquiredSemaphoreCreateInfo.flags = 0;

	V_CHK(vkCreateSemaphore(Factory->Device, &imageAcquiredSemaphoreCreateInfo, NULL, &SemaphoreWait));


}

VKRenderContext::~VKRenderContext()
{
	PreDestroy();
	vkDestroySemaphore(Factory->Device, SemaphoreWait, 0);
	vkDestroyFence(Factory->Device,Fence,0);
	VkCommandBuffer cmd_bufs[1] = { CommandBuffer };
	vkFreeCommandBuffers(Factory->Device, CommandPool, 1, cmd_bufs);
	vkDestroyCommandPool(Factory->Device, CommandPool, NULL);
}

void VKRenderContext::Wait()
{
	if (m_Status != 2)
		return;
	
	V_CHK(vkWaitForFences(Factory->Device, 1, &Fence, true, UINT64_MAX));
	V_CHK(vkResetFences(Factory->Device, 1, &Fence));

	if (!m_viewport.empty())static_cast<VKRenderViewport*>(m_viewport->GetHandle())->Swap();
	m_Status = 0;

}

void VKRenderContext::Flush(bool wait)
{
	static VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	if (m_Status != 1)
		return;
	vkCmdEndRenderPass(CommandBuffer);
	V_CHK(vkEndCommandBuffer(CommandBuffer));

	if (m_viewport.empty()==false)
	{
		auto viewport = static_cast<VKRenderViewport*>(m_viewport->GetHandle());
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &SemaphoreWait;
		submit_info.signalSemaphoreCount =1;
		submit_info.pSignalSemaphores = &viewport->Semaphore;// &buf.acquire_semaphore;
		submit_info.pWaitDstStageMask = &stage;
		submit_info.pCommandBuffers = &CommandBuffer;
		submit_info.commandBufferCount = 1;
		V_CHK(vkQueueSubmit(Factory->Queue, 1, &submit_info, Fence));
	}
	else
	{
		BEAR_ASSERT(false);
	}
	m_Status = 2;
	if (wait)Wait();

}



void VKRenderContext::AttachViewportAsFrameBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> Viewport)
{
	DetachFrameBuffer();
	m_viewport = Viewport;

}

void VKRenderContext::DetachFrameBuffer()
{
	PreDestroy();
	m_viewport.clear();
}

void VKRenderContext::ClearFrameBuffer()
{
	if (UpdateStatus() != 0)return;
	m_Status = 1;
	auto SwapChain = static_cast<VKRenderViewport*>(m_viewport->GetHandle());


	VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo;
	{
		CommandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		CommandBufferInheritanceInfo.pNext = nullptr;
		CommandBufferInheritanceInfo.renderPass = 0;
		CommandBufferInheritanceInfo.subpass = 0;
		CommandBufferInheritanceInfo.framebuffer = static_cast<VKRenderViewport*>(m_viewport->GetHandle())->Framebuffers[SwapChain->FrameIndex];
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
	vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	auto RpBegin = SwapChain->GetRenderPass();
	vkCmdBeginRenderPass(CommandBuffer, &RpBegin, VK_SUBPASS_CONTENTS_INLINE);

}

void VKRenderContext::PreDestroy()
{
	if (m_Status == 1)Flush(true);
	if (m_Status == 2)Wait();
}

int VKRenderContext::UpdateStatus()
{
	if (m_Status == 2)Wait();
	return m_Status;
}
