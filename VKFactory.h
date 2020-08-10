#pragma once
class VKFactory :public BearRHI::BearRHIFactory
{
	//BEAR_CLASS_WITHOUT_COPY(VKFactory);
public:
	VKFactory();
	virtual ~VKFactory();
	virtual BearRHI::BearRHIContext* CreateContext();
	virtual BearRHI::BearRHIViewport* CreateViewport( void* Handle, size_t Width, size_t Height, bool Fullscreen, bool VSync, const BearViewportDescription& Description);
	virtual BearRHI::BearRHIShader* CreateShader(BearShaderType Type);
	virtual BearRHI::BearRHIVertexBuffer* CreateVertexBuffer();
	virtual BearRHI::BearRHIIndexBuffer* CreateIndexBuffer();
	virtual BearRHI::BearRHIPipelineGraphics* CreatePipelineGraphics(const BearPipelineGraphicsDescription& Description);
	virtual BearRHI::BearRHIPipelineMesh* CreatePipelineMesh(const BearPipelineMeshDescription& Description);
	virtual BearRHI::BearRHIUniformBuffer* CreateUniformBuffer(size_t Stride, size_t Count, bool Dynamic);
	virtual BearRHI::BearRHIRootSignature* CreateRootSignature(const BearRootSignatureDescription& Description);
	virtual BearRHI::BearRHIDescriptorHeap* CreateDescriptorHeap(const BearDescriptorHeapDescription& Description);
	virtual BearRHI::BearRHITexture2D* CreateTexture2D(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data = 0);
	virtual BearRHI::BearRHITextureCube* CreateTextureCube(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data = 0);
	virtual BearRHI::BearRHIStructuredBuffer* CreateStructuredBuffer(size_t size, void* data = 0, bool UAV = false);

	virtual BearRHI::BearRHITexture2D* CreateTexture2D(size_t Width, size_t Height, BearRenderTargetFormat Format);
	virtual BearRHI::BearRHITexture2D* CreateTexture2D(size_t Width, size_t Height, BearDepthStencilFormat Format);
	virtual BearRHI::BearRHISampler* CreateSampler(const BearSamplerDescription& Description);
	virtual BearRHI::BearRHIRenderPass* CreateRenderPass(const BearRenderPassDescription& Description);
	virtual BearRHI::BearRHIFrameBuffer* CreateFrameBuffer(const BearFrameBufferDescription& Description);

	virtual BearRHI::BearRHIPipelineRayTracing* CreatePipelineRayTracing(const BearPipelineRayTracingDescription& Description);
	virtual BearRHI::BearRHIBottomLevel* CreateBottomLevel(const BearBottomLevelDescription& Description);
	virtual BearRHI::BearRHITopLevel* CreateTopLevel(const BearTopLevelDescription& Description);
	virtual BearRHI::BearRHIRayTracingShaderTable* CreateRayTracingShaderTable(const BearRayTracingShaderTableDescription& Description);

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
	virtual bool SupportRayTracing();
	virtual bool SupportMeshShader();
public:
	VkCommandBuffer CommandBuffer;
	void LockCommandBuffer();
	void UnlockCommandBuffer(const VkSemaphore* SignalSemaphores=0);
private:
	BearMutex m_CommandMutex;
	VkCommandPool m_CommandPool;
public:
	VkPhysicalDeviceFeatures DeviceFeatures;
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
#ifdef RTX
	VkPhysicalDeviceRayTracingPropertiesNV PhysicalDeviceRayTracingProperties;
#endif
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
	VkDebugUtilsMessengerEXT DebugMessenger;

	VkRenderPass RenderPass;
private:
	void LoadFunctions();
public:
#ifdef RTX
	IDxcCompiler3* DxcCompiler;
	IDxcLibrary* DxcLibrary;
#endif
};

extern VKFactory* Factory;