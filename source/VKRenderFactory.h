#pragma once
class VKRenderFactory :public BearRenderBase::BearRenderFactoryBase
{
	BEAR_CLASS_NO_COPY(VKRenderFactory);
public:
	VKRenderFactory();
	virtual ~VKRenderFactory();
	virtual BearRenderBase::BearRenderInterfaceBase* CreateInterface();
	virtual BearRenderBase::BearRenderContextBase* CreateContext();
	virtual BearRenderBase::BearRenderViewportBase* CreateViewport(void * Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync, const BearGraphics::BearRenderViewportDescription&Description);
	virtual BearRenderBase::BearRenderShaderBase* CreateShader(BearGraphics::BearShaderType Type);

	virtual BearRenderBase::BearRenderIndexBufferBase* CreateIndexBuffer();
	virtual BearRenderBase::BearRenderVertexBufferBase* CreateVertexBuffer();
	inline bool Empty()const { return Instance==0; }
public:
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	VkDevice Device;
	uint32_t QueueFamilyIndex;
	VkQueue Queue;
#ifdef DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
};