#pragma once
class VKTexture2D : public VKShaderResource, public BearRHI::BearRHITexture2D
{
public:
	VKTexture2D(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data = 0);
	VKTexture2D(size_t Width, size_t Height, BearRenderTargetFormat Format);
	VKTexture2D(size_t Width, size_t Height, BearDepthStencilFormat Format);
	VkImage Image;
	VkDeviceMemory ImageMemory;
	VkImageView ImageView;
	VkDescriptorImageInfo DescriptorImageInfo;
	VkImageCreateInfo ImageInfo;
	virtual ~VKTexture2D();
public:
	virtual void SetAsSRV(VkWriteDescriptorSet* HEAP, size_t offset);
	virtual void*QueryInterface(int Type);
	virtual BearTextureType GetType();
	virtual void* Lock(size_t mip, size_t depth);
	virtual void Unlock();
private:
	void* m_buffer;
	size_t m_mip;
	size_t m_depth;
private:
	BearTextureType TextureType;
	BearTextureUsage TextureUsage;
	BearTexturePixelFormat Format;
	BearRenderTargetFormat RTVFormat;
	BearDepthStencilFormat DSVFormat;
private:
	void AllocBuffer();
	void FreeBuffer();
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
};