#include "VKPCH.h"
size_t RenderPassCounter = 0;
VKRenderPass::VKRenderPass(const BearRenderPassDescription& description)
{
	RenderPassCounter++;
	Description = description;
	CountRenderTarget = 0;
	for (; Description.RenderTargets[CountRenderTarget].Format != RTF_NONE && CountRenderTarget < 8; CountRenderTarget++) {}


	VkAttachmentReference color_reference[8];


	VkAttachmentReference depth_reference = {};
	
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription attachments[9];
	VkSubpassDescription subpass;

	for (size_t i = 0; i < CountRenderTarget; i++)
	{
		attachments[i].format = VKFactory::Translation(Description.RenderTargets[i].Format);
		attachments[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[i].loadOp = Description.RenderTargets[i].Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		attachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[i].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[i].flags = 0;
		color_reference[i].attachment = static_cast<uint32_t>(i);
		color_reference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	}




	
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.colorAttachmentCount = static_cast<uint32_t>(CountRenderTarget); ;
	subpass.pColorAttachments = color_reference;
	subpass.pResolveAttachments = NULL;
	subpass.pDepthStencilAttachment = NULL;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;



	VkRenderPassCreateInfo RenderPassCreateInfo = {};
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.pNext = NULL;
	RenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(CountRenderTarget); ;
	RenderPassCreateInfo.pAttachments = attachments;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &subpass;
	RenderPassCreateInfo.dependencyCount = 0;
	RenderPassCreateInfo.pDependencies = NULL;

	if (Description.DepthStencil.Format != DSF_NONE)
	{
		attachments[CountRenderTarget].format = VKFactory::Translation(Description.DepthStencil.Format);
		attachments[CountRenderTarget].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[CountRenderTarget].loadOp = Description.DepthStencil.Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		attachments[CountRenderTarget].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		switch (Description.DepthStencil.Format)
		{
		case DSF_DEPTH24_STENCIL8:

		case DSF_DEPTH32F_STENCIL8:
			attachments[CountRenderTarget].stencilLoadOp = Description.DepthStencil.Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			attachments[CountRenderTarget].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			break;
		default:
			attachments[CountRenderTarget].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[CountRenderTarget].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			break;
		}

		attachments[CountRenderTarget].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[CountRenderTarget].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[CountRenderTarget].flags = 0;
		RenderPassCreateInfo.attachmentCount++;
		subpass.pDepthStencilAttachment = &depth_reference;
		depth_reference.attachment = static_cast<uint32_t>(CountRenderTarget); ;

	}


	V_CHK(vkCreateRenderPass(Factory->Device, &RenderPassCreateInfo, NULL, &RenderPass));
}

VKRenderPass::~VKRenderPass()
{
	vkDestroyRenderPass(Factory->Device, RenderPass, 0);
	RenderPassCounter--;
}
