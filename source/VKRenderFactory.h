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

	virtual  BearRenderBase::BearRenderPipelineBase*   CreatePipeline(const BearGraphics::BearRenderPipelineDescription&Descruotion);
	virtual BearRenderBase::BearRenderIndexBufferBase* CreateIndexBuffer();
	virtual BearRenderBase::BearRenderVertexBufferBase* CreateVertexBuffer();
	virtual BearRenderBase::BearRenderUniformBufferBase* CreateUniformBuffer();
	virtual BearRenderBase::BearRenderRootSignatureBase* CreateRootSignature(const BearGraphics::BearRenderRootSignatureDescription&Descruotion);
	virtual BearRenderBase::BearRenderDescriptorHeapBase* CreateDescriptorHeap(const BearGraphics::BearRenderDescriptorHeapDescription&Description);
	virtual BearRenderBase::BearRenderSamplerStateBase* CreateSamplerState( const BearGraphics::BearRenderSamplerDescription&Description);
	virtual BearRenderBase::BearRenderTexture2DBase* CreateTexture2D();
	inline bool Empty()const { return Instance==0; }
public:
	VkPipelineCache PipelineCacheDefault;
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	VkDevice Device;
	uint32_t QueueFamilyIndex;
	VkQueue Queue;
#ifdef DEBUG
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
public:
	VkCommandBuffer CommandBuffer;
	void LockCommandBuffer();
	void UnlockCommandBuffer();
private:
	BearMutex m_CommandMutex;
	VkCommandPool m_CommandPool;


};