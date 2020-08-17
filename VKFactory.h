#pragma once
class VKFactory :public BearRHI::BearRHIFactory
{
	BEAR_CLASS_WITHOUT_COPY(VKFactory);
private:
	bool LoadFunctions();
	bool CreateInstance();
	bool CreateDevice();
	bool CreateGPU(uint32_t& queue_family_index);
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
	virtual BearRHI::BearRHIRayTracingBottomLevel* CreateRayTracingBottomLevel(const BearRayTracingBottomLevelDescription& Description);
	virtual BearRHI::BearRHIRayTracingTopLevel* CreateRayTracingTopLevel(const BearRayTracingTopLevelDescription& Description);
	virtual BearRHI::BearRHIRayTracingShaderTable* CreateRayTracingShaderTable(const BearRayTracingShaderTableDescription& Description);

	static VkSamplerAddressMode Translation(BearSamplerAddressMode format);
	static VkCullModeFlagBits Translation(BearRasterizerCullMode format);
	static VkPolygonMode Translation(BearRasterizerFillMode format);
	static VkFrontFace Translation(BearRasterizerFrontFace format);

	static VkBlendFactor Translation(BearBlendFactor format);
	static VkBlendOp Translation(BearBlendOp format);
	static VkCompareOp Translation(BearCompareFunction format);
	static VkStencilOp Translation(BearStencilOp format);

	static VkFormat  Translation(BearTexturePixelFormat format);
	static VkFormat  Translation(BearRenderTargetFormat format);
	static VkFormat  Translation(BearDepthStencilFormat format);
public:
	inline bool Empty()const { return Instance==0; }
	virtual bool SupportRayTracing();
	virtual bool SupportMeshShader();
public:
	VkCommandBuffer CommandBuffer;
	void LockCommandBuffer();
	void UnlockCommandBuffer();
private:
	BearMutex m_CommandMutex;
	VkCommandPool m_CommandPool;
public:
	VkPhysicalDeviceFeatures DeviceFeatures;
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
#ifdef RTX
	VkPhysicalDeviceRayTracingPropertiesNV PhysicalDeviceRayTracingProperties;
#endif
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
public:
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
public:
	uint32_t QueueFamilyIndex;
	VkQueue Queue;
	VkSemaphore SemaphoreWait;
	VkFence Fence;
private:
#ifdef DEVELOPER_VERSION
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
public:
#ifdef RTX
	IDxcCompiler3* DxcCompiler;
	IDxcLibrary* DxcLibrary;
#endif
private:
	bool bSupportRayTracing;
};

extern VKFactory* Factory;