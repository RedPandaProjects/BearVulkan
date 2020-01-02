#pragma once
#include "BearRenderBase.h"
#ifdef WINDOWS
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif
#include "vulkan/vulkan.h"
#define V_CHK(a) BEAR_ASSERT(a==VK_SUCCESS)
#include "VKFactory.h"
extern VKFactory* Factory;
#include "VKViewport.h"
#include "VKContext.h"