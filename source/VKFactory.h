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
	virtual BearRHI::BearRHIUniformBuffer* CreateUniformBuffer();
	virtual BearRHI::BearRHIRootSignature* CreateRootSignature(const BearRootSignatureDescription& Description);
	virtual BearRHI::BearRHIDescriptorHeap* CreateDescriptorHeap(const BearDescriptorHeapDescription& Description);
	virtual BearRHI::BearRHITexture2D* CreateTexture2D(bsize Width, bsize Height, bsize Mips, bsize Count, BearTexturePixelFormat PixelFormat, void* data = 0);
	virtual BearRHI::BearRHISampler* CreateSampler();
	inline bool Empty()const { return Instance==0; }
public:

	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
	VkPipelineCache PipelineCacheDefault;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	VkPipelineLayout PipelineLayout;
	uint32_t QueueFamilyIndex;
	VkQueue Queue;
	
#ifdef DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif


};