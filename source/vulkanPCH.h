#pragma once
#include "SPIRV/GlslangToSpv.h"
#include "BearRenderBase.h"
#ifdef WINDOWS
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif
#include "vulkan/vulkan.h"

using namespace BearCore;
#include "VKRenderInterface.h"
#include "VKRenderViewport.h"
#include "VKRenderContext.h"
#include "VKRenderFactory.h"

#include "VKRenderIndexBuffer.h"
#include "VKRenderVertexBuffer.h"
#include "VKRenderUniformBuffer.h"

#include "VKRenderPipeline.h"
#include "VKRenderShader.h"

extern VKRenderFactory *Factory;
#define V_CHK(a) BEAR_ASSERT(a==VK_SUCCESS)