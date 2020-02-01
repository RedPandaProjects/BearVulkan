#include "VKPCH.h"
bsize PipelineCounter = 0;
inline VkFormat TranslateVertexFormat(BearVertexFormat format)
{
	switch (format)
	{
	case VF_R32G32B32A32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
	case VF_R32G32B32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
	case VF_R32G32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32_SFLOAT;
	case VF_R32_FLOAT:
		return VkFormat::VK_FORMAT_R32_SFLOAT;

	case VF_R32_INT:
		return VkFormat::VK_FORMAT_R32_SINT;
	case VF_R8G8B8A8:
		return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
	case VF_R8G8:
		return VkFormat::VK_FORMAT_R8G8_UINT;
	case VF_R8:
		return VkFormat::VK_FORMAT_R8_UINT;
	default:
		BEAR_ASSERT(0);;
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	//return DXGI_FORMAT_UNKNOWN;
}
inline bsize VertexFormatToSize(BearVertexFormat format)
{
	switch (format)
	{
	case VF_R32G32B32A32_FLOAT:
		return sizeof(float) * 4;
	case VF_R32G32B32_FLOAT:
		return sizeof(float) * 3;
	case VF_R32G32_FLOAT:
		return sizeof(float) * 2;
	case VF_R32_FLOAT:
		return sizeof(float);

	case VF_R32_INT:
		return sizeof(int32);
	case VF_R8G8B8A8:
		return sizeof(int32);

	case VF_R8G8:
		return sizeof(uint16);
	case VF_R8:
		return sizeof(uint8);
	default:
		BEAR_ASSERT(0);;
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	//return DXGI_FORMAT_UNKNOWN;
}
VKPipeline::VKPipeline(const BearPipelineDescription& desc)
{
	PipelineCounter++;
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo DynamicState = {};
	{
		memset(dynamicStateEnables, 0, sizeof(dynamicStateEnables));
		DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicState.pNext = NULL;
		DynamicState.pDynamicStates = dynamicStateEnables;
		DynamicState.dynamicStateCount = 0;
	}

	VkPipelineVertexInputStateCreateInfo VertexInputState;
	VkVertexInputBindingDescription VertexInputBinding[2];
	VkVertexInputAttributeDescription   VertexInputAttribute[16];
	{
		bsize  VertexInputCount = 0;
		{
			bsize Stride = 0;
			VertexInputBinding[0].binding = 0;
			VertexInputBinding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


			VertexInputBinding[1].binding = 1;
			VertexInputBinding[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

			for (bsize i = 0; i < 16 && !desc.InputLayout.Elements[i].empty(); i++)
			{
				auto& cElement = desc.InputLayout.Elements[i];
				VertexInputAttribute[i].offset = static_cast<UINT>(desc.InputLayout.Elements[i].Offset);;
				VertexInputAttribute[i].location = static_cast<uint32>(i);
				VertexInputAttribute[i].format = TranslateVertexFormat(cElement.Type);
				VertexInputAttribute[i].binding = desc.InputLayout.Elements[i].IsInstance ? 1 : 0;
				Stride = BearMath::max(VertexFormatToSize(cElement.Type) + static_cast<bsize>(VertexInputAttribute[i].offset), Stride);
				VertexInputCount++;
			}
			VertexInputBinding[0].stride = static_cast<uint32>(Stride);
			VertexInputBinding[1].stride = static_cast<uint32>(Stride);;
		}

		VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VertexInputState.pNext = NULL;
		VertexInputState.flags = 0;
		VertexInputState.vertexBindingDescriptionCount = 2;
		VertexInputState.pVertexBindingDescriptions = VertexInputBinding;
		VertexInputState.vertexAttributeDescriptionCount = static_cast<uint32>(VertexInputCount); ;
		VertexInputState.pVertexAttributeDescriptions = VertexInputAttribute;
	}

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyState;
	{
		InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyState.pNext = NULL;
		InputAssemblyState.flags = 0;
		InputAssemblyState.primitiveRestartEnable = VK_FALSE;
		switch (desc.TopologyType)
		{
		case TT_POINT_LIST:
			InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			break;
		case TT_LINE_LIST:
			InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;
		case TT_TRIANGLE_LIST:
			InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;
		default:
			break;
		}

	}

	VkPipelineRasterizationStateCreateInfo RasterizationState;
	{
		RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizationState.pNext = NULL;
		RasterizationState.flags = 0;
		RasterizationState.polygonMode = VKFactory::Translate(desc.RasterizerState.FillMode);
		RasterizationState.cullMode = VKFactory::Translate(desc.RasterizerState.CullMode);;
		RasterizationState.frontFace = VKFactory::Translate(desc.RasterizerState.FrontFace);;;
		RasterizationState.depthClampEnable = VK_FALSE;
		RasterizationState.rasterizerDiscardEnable = VK_FALSE;
		RasterizationState.depthBiasEnable =VK_TRUE;
		RasterizationState.depthBiasConstantFactor = desc.RasterizerState.DepthBias;
		RasterizationState.depthBiasClamp = 0;
		RasterizationState.depthBiasSlopeFactor = desc.RasterizerState.SlopeScaleDepthBias;
		RasterizationState.lineWidth = 1.0f;
	}

	VkPipelineColorBlendStateCreateInfo BlendState;
	VkPipelineColorBlendAttachmentState BlendAttachment[8];
	{
		BlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		BlendState.pNext = NULL;
		BlendState.flags = 0;
		for (bsize i = 0; i < 8; i++)
		{
			BlendAttachment[i].blendEnable = desc.BlendState.RenderTarget[i].Enable;
			BlendAttachment[i].alphaBlendOp = VKFactory::Translate(desc.BlendState.RenderTarget[i].Alpha);
			BlendAttachment[i].colorBlendOp = VKFactory::Translate(desc.BlendState.RenderTarget[i].Color);
			BlendAttachment[i].srcColorBlendFactor = VKFactory::Translate(desc.BlendState.RenderTarget[i].ColorSrc);
			BlendAttachment[i].dstColorBlendFactor = VKFactory::Translate(desc.BlendState.RenderTarget[i].ColorDst);
			BlendAttachment[i].srcAlphaBlendFactor = VKFactory::Translate(desc.BlendState.RenderTarget[i].AlphaSrc);
			BlendAttachment[i].dstAlphaBlendFactor = VKFactory::Translate(desc.BlendState.RenderTarget[i].AlphaDst);
			BlendAttachment[i].colorWriteMask = 0;
			if (desc.BlendState.RenderTarget[i].ColorWriteMask & CWM_R)
				BlendAttachment[i].colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
			if (desc.BlendState.RenderTarget[i].ColorWriteMask & CWM_G)
				BlendAttachment[i].colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
			if (desc.BlendState.RenderTarget[i].ColorWriteMask & CWM_B)
				BlendAttachment[i].colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
			if (desc.BlendState.RenderTarget[i].ColorWriteMask & CWM_A)
				BlendAttachment[i].colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
		}
	
		
		
		BlendState.attachmentCount = 8;
		BlendState.pAttachments = BlendAttachment;
		BlendState.logicOpEnable = VK_FALSE;
		BlendState.logicOp = VK_LOGIC_OP_NO_OP;
		BlendState.blendConstants[0] = 1.0f;
		BlendState.blendConstants[1] = 1.0f;
		BlendState.blendConstants[2] = 1.0f;
		BlendState.blendConstants[3] = 1.0f;
	}


	VkPipelineViewportStateCreateInfo ViewportState = {};
	{
		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.pNext = NULL;
		ViewportState.flags = 0;
		ViewportState.viewportCount = 1;
		dynamicStateEnables[DynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
		ViewportState.scissorCount = 1;
		dynamicStateEnables[DynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
		ViewportState.pScissors = NULL;
		ViewportState.pViewports = NULL;
	}

	VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	{
		DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencilState.pNext = NULL;
		DepthStencilState.flags = 0;
		DepthStencilState.depthTestEnable = desc.DepthStencilState.DepthEnable;
		DepthStencilState.depthWriteEnable = desc.DepthStencilState.EnableDepthWrite;
		DepthStencilState.depthCompareOp = VKFactory::Translate(desc.DepthStencilState.DepthTest);
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.minDepthBounds = 0;
		DepthStencilState.maxDepthBounds = 0;
		DepthStencilState.stencilTestEnable = desc.DepthStencilState.StencillEnable;

		if (!desc.DepthStencilState.BackStencillEnable)
		{
			DepthStencilState.front.failOp = VKFactory::Translate(desc.DepthStencilState.FrontFace.StencilFailOp);
			DepthStencilState.front.passOp = VKFactory::Translate(desc.DepthStencilState.FrontFace.StencilPassOp);
			DepthStencilState.front.compareOp = VKFactory::Translate(desc.DepthStencilState.FrontFace.StencilTest);
			DepthStencilState.front.compareMask = desc.DepthStencilState.StencilReadMask;
			DepthStencilState.front.reference = 0;
			DepthStencilState.front.depthFailOp = VKFactory::Translate(desc.DepthStencilState.FrontFace.StencilDepthFailOp);;
			DepthStencilState.front.writeMask = desc.DepthStencilState.StencilWriteMask;;
			DepthStencilState.back = DepthStencilState.front;
		}
		else
		{
			DepthStencilState.back.failOp = VKFactory::Translate(desc.DepthStencilState.BackFace.StencilFailOp);
			DepthStencilState.back.passOp = VKFactory::Translate(desc.DepthStencilState.BackFace.StencilPassOp);
			DepthStencilState.back.compareOp = VKFactory::Translate(desc.DepthStencilState.BackFace.StencilTest);
			DepthStencilState.back.compareMask = desc.DepthStencilState.StencilReadMask;
			DepthStencilState.back.reference = 0;
			DepthStencilState.back.depthFailOp = VKFactory::Translate(desc.DepthStencilState.BackFace.StencilDepthFailOp);;
			DepthStencilState.back.writeMask = desc.DepthStencilState.StencilWriteMask;;
		}
	}

	VkPipelineMultisampleStateCreateInfo MultisampleState;
	{
		MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultisampleState.pNext = NULL;
		MultisampleState.flags = 0;
		MultisampleState.pSampleMask = NULL;
		MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		MultisampleState.sampleShadingEnable = VK_FALSE;
		MultisampleState.alphaToCoverageEnable = VK_FALSE;
		MultisampleState.alphaToOneEnable = VK_FALSE;
		MultisampleState.minSampleShading = 0.0;
	}
	VkPipelineShaderStageCreateInfo ShaderStage[6];
	bsize CountShader = 0;
	{
	
		auto vs = static_cast<const VKShader*>(desc.Shaders.Vertex.get());
		if (vs && vs->IsType(ST_Vertex))
		{
			bear_copy(&ShaderStage[CountShader], &vs->Shader, 1);
			CountShader++;
		}
		auto ps = static_cast<const VKShader*>(desc.Shaders.Pixel.get());
		if (ps && ps->IsType(ST_Pixel))
		{
			bear_copy(&ShaderStage[CountShader], &ps->Shader, 1);
			CountShader++;
		}
	}
	RootSignature = desc.RootSignature;

	BEAR_ASSERT(RootSignature.empty() == false);
	RootSignaturePointer = static_cast<VKRootSignature*>(RootSignature.get());
	VkGraphicsPipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext = NULL;
	info.layout = RootSignaturePointer->PipelineLayout;// RootSignaturePointer->PipelineLayout;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = 0;
	info.flags = 0;
	info.pVertexInputState = &VertexInputState;
	info.pInputAssemblyState = &InputAssemblyState;
	info.pRasterizationState = &RasterizationState;
	info.pColorBlendState = &BlendState;
	info.pTessellationState = NULL;
	info.pMultisampleState = &MultisampleState;
	info.pDynamicState = &DynamicState;
	info.pViewportState = &ViewportState;
	info.pDepthStencilState = &DepthStencilState;
	info.pStages = ShaderStage;
	if (desc.RenderPass.empty())
		info.renderPass = Factory->RenderPass;
	else
		info.renderPass = static_cast<const VKRenderPass*>(desc.RenderPass.get())->RenderPass;
	info.stageCount = static_cast<uint32>(CountShader); 
	info.subpass = 0;
	V_CHK(vkCreateGraphicsPipelines(Factory->Device, Factory->PipelineCacheDefault, 1, &info, NULL, &Pipeline));
}

VKPipeline::~VKPipeline()
{
	PipelineCounter--;
}
