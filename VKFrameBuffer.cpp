#include "VKPCH.h"
size_t FrameBufferCounter = 0;
VKFrameBuffer::VKFrameBuffer(const BearFrameBufferDescription& description):Description(description)
{
	FrameBufferCounter++;
	BEAR_CHECK(!description.RenderPass.empty());
	RenderPassRef = static_cast<VKRenderPass*>(Description.RenderPass.get());
	 CountRenderTarget = 0;

	VkFramebufferCreateInfo FramebufferCreateInfo = {};
	VkImageView attachments[9];
	for (; CountRenderTarget < 8; CountRenderTarget++)
	{
		if (Description.RenderTargets[CountRenderTarget].empty())
		{
			break;
		}
		
		auto textures = static_cast<VKTexture2D*>(Description.RenderTargets[CountRenderTarget].get());
		BEAR_CHECK(textures->GetType() == TT_RenderTarget);
		FramebufferCreateInfo.width = BearMath::max(textures->ImageInfo.extent.width, FramebufferCreateInfo.width);
		FramebufferCreateInfo.height = BearMath::max(textures->ImageInfo.extent.height, FramebufferCreateInfo.height);
		attachments[CountRenderTarget] = textures->ImageView;
	
	}

	BEAR_CHECK(RenderPassRef->CountRenderTarget == CountRenderTarget);
	

	if (Description.DepthStencil.empty())
	{
		BEAR_CHECK(RenderPassRef->Description.DepthStencil.Format == DSF_NONE);
	}
	else
	{
		BEAR_CHECK(Description.DepthStencil.get()->GetType() == TT_DepthStencil);
		auto textures = static_cast<VKTexture2D*>(Description.DepthStencil.get());
		FramebufferCreateInfo.width = BearMath::max(textures->ImageInfo.extent.width, FramebufferCreateInfo.width);
		FramebufferCreateInfo.height = BearMath::max(textures->ImageInfo.extent.height, FramebufferCreateInfo.height);
		attachments[CountRenderTarget] = textures->ImageView;
		
	}
	Width = FramebufferCreateInfo.width;
	Height = FramebufferCreateInfo.height;
	FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferCreateInfo.pNext = NULL;
	FramebufferCreateInfo.renderPass = RenderPassRef->RenderPass;
	FramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(CountRenderTarget + (Description.DepthStencil.empty()?0:1));
	FramebufferCreateInfo.pAttachments = attachments;
	FramebufferCreateInfo.layers = 1;
	V_CHK( vkCreateFramebuffer(Factory->Device, &FramebufferCreateInfo, 0, &FrameBuffer));
}

VKFrameBuffer::~VKFrameBuffer()
{
	vkDestroyFramebuffer(Factory->Device, FrameBuffer, 0);
	FrameBufferCounter--;
}
VkRenderPassBeginInfo VKFrameBuffer::GetRenderPass()
{
	VkRenderPassBeginInfo rp_begin = {};
	rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_begin.pNext = NULL;
	rp_begin.renderPass = RenderPassRef->RenderPass;
	rp_begin.framebuffer = FrameBuffer;
	rp_begin.renderArea.offset.x = 0;
	rp_begin.renderArea.offset.y = 0;
	rp_begin.renderArea.extent.width = static_cast<uint32_t>(Width);
	rp_begin.renderArea.extent.height = static_cast<uint32_t>(Height);
	rp_begin.clearValueCount = static_cast<uint32_t>(CountRenderTarget);
	for (size_t i = 0; i < CountRenderTarget; i++)
	{
		ClearValues[i].color.float32[0] = RenderPassRef->Description.RenderTargets[i].Color.R32F;
		ClearValues[i].color.float32[1] = RenderPassRef->Description.RenderTargets[i].Color.G32F;
		ClearValues[i].color.float32[2] = RenderPassRef->Description.RenderTargets[i].Color.B32F;
		ClearValues[i].color.float32[3] = RenderPassRef->Description.RenderTargets[i].Color.A32F;
	}
	if (!Description.DepthStencil.empty())
	{
		ClearValues[CountRenderTarget].depthStencil.depth = RenderPassRef->Description.DepthStencil.Depth;
		ClearValues[CountRenderTarget].depthStencil.stencil = RenderPassRef->Description.DepthStencil.Stencil;
		rp_begin.clearValueCount++;
	}
	rp_begin.pClearValues = ClearValues;
	return rp_begin;
}

void VKFrameBuffer::Unlock(VkCommandBuffer Cmd)
{

	for (size_t i = 0; CountRenderTarget > i; i++)
	{
		auto texture = static_cast<VKTexture2D*>(Description.RenderTargets[i].get());
		TransitionImageLayout(Cmd, texture->Image, texture->ImageInfo.format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture->ImageInfo.mipLevels, texture-> ImageInfo.arrayLayers,0);
	}
}

void VKFrameBuffer::Lock(VkCommandBuffer Cmd)
{
	for (size_t i = 0; CountRenderTarget > i; i++)
	{
		auto texture = static_cast<VKTexture2D*>(Description.RenderTargets[i].get());
		TransitionImageLayout(Cmd, texture->Image, texture->ImageInfo.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, texture->ImageInfo.mipLevels, texture-> ImageInfo.arrayLayers,0);
	}
}
