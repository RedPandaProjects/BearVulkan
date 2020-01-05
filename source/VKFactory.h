#pragma once
class VKFactory :public BearRHI::BearRHIFactory
{
	BEAR_CLASS_WITHOUT_COPY(VKFactory);
public:
	VKFactory();
	virtual ~VKFactory();
	virtual BearRHI::BearRHIContext* CreateContext();
	virtual BearRHI::BearRHIViewport* CreateViewport( void* Handle, bsize Width, bsize Height, bool Fullscreen, bool VSync, const BearViewportDescription& Description);
	virtual BearRHI::BearRHIShader* CreateShader(BearShaderType Type);
	virtual BearRHI::BearRHIVertexBuffer* CreateVertexBuffer();
	virtual BearRHI::BearRHIIndexBuffer* CreateIndexBuffer();
	virtual BearRHI::BearRHIPipeline* CreatePipeline(const BearPipelineDescription& Description);

	inline bool Empty()const { return Instance==0; }
public:
	VkPipelineCache PipelineCacheDefault;
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	VkDevice Device;
	uint32_t QueueFamilyIndex;
	VkQueue Queue;
	VkPipelineLayout PipelineLayout;
#ifdef DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif


};