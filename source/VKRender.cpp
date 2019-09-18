#include "vulkanPCH.h"
VKRenderFactory *Factory = 0;
void RenderInitialize()
{
	Factory = BearCore::bear_new<VKRenderFactory>();
	if (!Factory->Empty())
	{
		GRenderFactoty = Factory;
		BEAR_ASSERT(GRenderFactoty);
		return;
	}
	BearCore::bear_delete(Factory);
	GRenderFactoty = 0;

}
void RenderDestroy()
{
	if (GRenderFactoty)
	{
		BearCore::bear_delete(Factory);
		GRenderFactoty = 0;
	}
}
