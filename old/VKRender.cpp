#include "vulkanPCH.h"
#include "glslang/Public/ShaderLang.h"
VKRenderFactory *Factory = 0;
void RenderInitialize()
{
	Factory = BearCore::bear_new<VKRenderFactory>();
	if (!Factory->Empty())
	{
		GRenderFactoty = Factory;
		BEAR_ASSERT(GRenderFactoty);
		glslang::InitializeProcess();
		return;
	}
	BearCore::bear_delete(Factory);
	GRenderFactoty = 0;

}
void RenderDestroy()
{
	if (GRenderFactoty)
	{
		glslang::FinalizeProcess();
		BearCore::bear_delete(Factory);
		GRenderFactoty = 0;
	}
}
