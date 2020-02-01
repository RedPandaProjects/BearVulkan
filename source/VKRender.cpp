#include "VKPCH.h"
VKFactory* Factory;
bool RHIInitialize()
{
	 Factory = bear_new<VKFactory>();
	if (!Factory->Empty())
	{
		GFactory = Factory;
		GStats = bear_new<VKStats>();
		BEAR_ASSERT(GFactory);
		return true;
	}
	bear_delete(Factory);
	GFactory = 0;
	return false;
}
