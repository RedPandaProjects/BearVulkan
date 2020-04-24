#pragma once

#include "../BearGraphics/BearRenderBase.h"
#ifdef DEVELOPER_VERSION
#ifdef RTX
#define NV_EXTENSIONS
#endif
#include "shaderc/shaderc.hpp"
#endif
#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif
#include "vulkan/vulkan.h"
extern void vk_error(VkResult result);
#define V_CHK(a) { VkResult __result_ = a ;if(__result_!=VK_SUCCESS)vk_error(__result_); }

enum EVKQuery
{
	VKQ_None = 0,
	VKQ_ShaderResource,
	VKQ_Pipeline,
};

#include "VKFactory.h"
extern VKFactory* Factory;
#include "VKViewport.h"
#include "VKContext.h"
#include "VKShader.h"
#include "VKBufferTools.h"
#include "VKVertexBuffer.h"
#include "VKIndexBuffer.h"

#include "VKUniformBuffer.h"

#include "VKRootSignature.h"
#include "VKPipeline.h"
#include "VKPipelineGraphics.h"
#include "VKPipelineMesh.h"
#include "VKDescriptorHeap.h"

#include "VKSamplers.h"
#include "VKShaderResource.h"
#include "VKTexture2D.h"
#include "VKStats.h"

#include "VKRenderPass.h" 
#include "VKFrameBuffer.h"
#include "VKTextureCube.h"
#include "VKStructuredBuffer.h"