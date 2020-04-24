#pragma once 
#define RENDER_BEGIN_CLASS_REGISTRATION1(Name,...) virtual size_t Count ## Name();
#define RENDER_BEGIN_CLASS_REGISTRATION2(Name,Parent,...) RENDER_BEGIN_CLASS_REGISTRATION1(Name)

#define RENDER_BEGIN_CLASS_REGISTRATION1_WITHOUT_FACTORY(Name,...)
#define RENDER_BEGIN_CLASS_REGISTRATION2_WITHOUT_FACTORY(Name,Parent,...) 

class   VKStats :public BearRHI::BearRHIStats
{
public:
#include "..\BearGraphics\BearTemplate\BearGraphicsObjectsList.h"
	virtual ~VKStats();
};