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
	virtual BearRHI::BearRHITexture2D* CreateTexture2D(bsize Width, bsize Height, bsize Mips, bsize Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data = 0);
	virtual BearRHI::BearRHITexture2D* CreateTexture2D(bsize Width, bsize Height, BearRenderTargetFormat Format);
	virtual BearRHI::BearRHITexture2D* CreateTexture2D(bsize Width, bsize Height, BearDepthStencilFormat Format);
	virtual BearRHI::BearRHISampler* CreateSampler(const BearSamplerDescription& Description);
	virtual BearRHI::BearRHIRenderPass* CreateRenderPass(const BearRenderPassDescription& Description);
	virtual BearRHI::BearRHIFrameBuffer* CreateFrameBuffer(const BearFrameBufferDescription& Description);

	static VkSamplerAddressMode Translation(BearSamplerAddressMode format);
	static VkCullModeFlagBits Translate(BearRasterizerCullMode format);
	static VkPolygonMode Translate(BearRasterizerFillMode format);
	static VkFrontFace Translate(BearRasterizerFrontFace format);

	static VkBlendFactor Translate(BearBlendFactor format);
	static VkBlendOp Translate(BearBlendOp format);
	static VkCompareOp Translate(BearCompareFunction format);
	static VkStencilOp Translate(BearStencilOp format);

	static VkFormat  Translation(BearTexturePixelFormat format);
	static VkFormat  Translation(BearRenderTargetFormat format);
	static VkFormat  Translation(BearDepthStencilFormat format);
	inline bool Empty()const { return Instance==0; }
public:
	VkCommandBuffer CommandBuffer;
	void LockCommandBuffer();
	void UnlockCommandBuffer(const VkSemaphore* SignalSemaphores=0);
private:
	BearMutex m_CommandMutex;
	VkCommandPool m_CommandPool;
public:
	VkSampler DefaultSampler;
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
	VkPipelineCache PipelineCacheDefault;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	VkPipelineLayout PipelineLayout;
	uint32_t QueueFamilyIndex;
	VkQueue Queue;

	VkSemaphore SemaphoreWait;
	VkFence Fence;
#ifdef DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif

	VkRenderPass RenderPass;
};