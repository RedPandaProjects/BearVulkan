#include "vulkanPCH.h"

VKRenderContext::VKRenderContext()
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

}

VKRenderContext::~VKRenderContext()
{
	VkCommandBuffer cmd_bufs[1] = { CommandBuffer };
	vkFreeCommandBuffers(Factory->Device, CommandPool, 1, cmd_bufs);
	vkDestroyCommandPool(Factory->Device, CommandPool, NULL);
}

void VKRenderContext::Flush()
{
	vkCmdEndRenderPass(CommandBuffer);
	V_CHK(vkEndCommandBuffer(CommandBuffer));
	if (!m_viewport.empty())static_cast<VKRenderViewport*>(m_viewport->GetHandle())->Swap(CommandBuffer);
}

void VKRenderContext::AttachViewportAsFrameBuffer(BearGraphics::BearFactoryPointer<BearRenderBase::BearRenderViewportBase> Viewport)
{
//	DetachFrameBuffer();
	m_viewport = Viewport;
	

}

void VKRenderContext::DetachFrameBuffer()
{
	vkCmdEndRenderPass(CommandBuffer);
	V_CHK(vkEndCommandBuffer(CommandBuffer));
}

void VKRenderContext::ClearFrameBuffer()
{
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
