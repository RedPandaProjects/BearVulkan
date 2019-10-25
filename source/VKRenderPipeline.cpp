#include "vulkanPCH.h"
inline VkFormat TranslateVertexFormat(BearGraphics::BearVertexFormat format)
{
	switch (format)
	{
	case BearGraphics::VF_R32G32B32A32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
	case BearGraphics::VF_R32G32B32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
	case BearGraphics::VF_R32G32_FLOAT:
		return VkFormat::VK_FORMAT_R32G32_SFLOAT;
	case BearGraphics::VF_R32_FLOAT:
		return VkFormat::VK_FORMAT_R32_SFLOAT;

	case BearGraphics::VF_R32_INT:
		return VkFormat::VK_FORMAT_R32_SINT;
	case BearGraphics::VF_R8G8B8A8:
		return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
	case BearGraphics::VF_R8G8:
		return VkFormat::VK_FORMAT_R8G8_UINT;
	case BearGraphics::VF_R8:
		return VkFormat::VK_FORMAT_R8_UINT;
	default:
		BEAR_ASSERT(0);;
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	//return DXGI_FORMAT_UNKNOWN;
}
inline bsize VertexFormatToSize(BearGraphics::BearVertexFormat format)
{
	switch (format)
	{
	case BearGraphics::VF_R32G32B32A32_FLOAT:
		return sizeof(float)*4;
	case BearGraphics::VF_R32G32B32_FLOAT:
		return sizeof(float) * 3;
	case BearGraphics::VF_R32G32_FLOAT:
		return sizeof(float) * 2;
	case BearGraphics::VF_R32_FLOAT:
		return sizeof(float) ;

	case BearGraphics::VF_R32_INT:
		return sizeof(int32);
	case BearGraphics::VF_R8G8B8A8:
		return sizeof(int32);

	case BearGraphics::VF_R8G8:
		return sizeof(uint16);
	case BearGraphics::VF_R8:
		return sizeof(uint8);
	default:
		BEAR_ASSERT(0);;
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	//return DXGI_FORMAT_UNKNOWN;
}
VKRenderPipeline::VKRenderPipeline(const BearGraphics::BearRenderPipelineDescription & desc)
{

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
				VertexInputAttribute[i].location =static_cast<uint32>( i);
				VertexInputAttribute[i].format = TranslateVertexFormat(cElement.Type);
				VertexInputAttribute[i].binding = desc.InputLayout.Elements[i].IsInstance ? 1 : 0;
				Stride = bear_max(VertexFormatToSize(cElement.Type) + VertexInputAttribute[i].offset, Stride);
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
		case BearGraphics::TT_POINT_LIST :
			InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			break;
		case BearGraphics::TT_LINE_LIST:
			InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;
		case BearGraphics::TT_TRIANGLE_LIST:
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
		RasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		RasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
		RasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		RasterizationState.depthClampEnable = VK_FALSE;
		RasterizationState.rasterizerDiscardEnable = VK_FALSE;
		RasterizationState.depthBiasEnable = VK_FALSE;
		RasterizationState.depthBiasConstantFactor = 0;
		RasterizationState.depthBiasClamp = 0;
		RasterizationState.depthBiasSlopeFactor = 0;
		RasterizationState.lineWidth = 1.0f;
	}

	VkPipelineColorBlendStateCreateInfo BlendState;
	VkPipelineColorBlendAttachmentState BlendAttachment[1];
	{
		BlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		BlendState.pNext = NULL;
		BlendState.flags = 0;

		BlendAttachment[0].colorWriteMask = 0xf;
		BlendAttachment[0].blendEnable = VK_FALSE;
		BlendAttachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
		BlendAttachment[0].colorBlendOp = VK_BLEND_OP_ADD;
		BlendAttachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		BlendAttachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		BlendAttachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		BlendAttachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		BlendState.attachmentCount = 1;
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
		DepthStencilState.depthTestEnable = VK_FALSE;
		DepthStencilState.depthWriteEnable = VK_FALSE;
		DepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.minDepthBounds = 0;
		DepthStencilState.maxDepthBounds = 0;
		DepthStencilState.stencilTestEnable = VK_FALSE;
		DepthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
		DepthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
		DepthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		DepthStencilState.back.compareMask = 0;
		DepthStencilState.back.reference = 0;
		DepthStencilState.back.depthFailOp = VK_STENCIL_OP_KEEP;
		DepthStencilState.back.writeMask = 0;
		DepthStencilState.front = DepthStencilState.back;
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
		auto ps = static_cast<const VKRenderShader*>(desc.Shaders.Pixel.get());
		if (ps&&ps->IsType(BearGraphics::ST_Pixel))
		{
			bear_copy(&ShaderStage[CountShader],& ps->Shader,1);
			CountShader++;
		}
		auto vs = static_cast<const VKRenderShader*>(desc.Shaders.Vertex.get());
		if (vs&&vs->IsType(BearGraphics::ST_Vertex))
		{
			bear_copy(&ShaderStage[CountShader], &vs->Shader, 1);
			CountShader++;
		}
	}
	VkGraphicsPipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext = NULL;
	info.layout = VK_NULL_HANDLE;
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
	info.stageCount = static_cast<uint32>(CountShader); ; ;
	info.subpass = 0;

	V_CHK(vkCreateGraphicsPipelines(Factory->Device, Factory->PipelineCacheDefault, 1, &info, NULL, &Pipeline));
}

VKRenderPipeline::~VKRenderPipeline()
{
	if (Pipeline)
		vkDestroyPipeline(Factory->Device, Pipeline, 0);
}
