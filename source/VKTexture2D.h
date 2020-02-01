#pragma once
class VKTexture2D : public VKShaderResource, public BearRHI::BearRHITexture2D
{
public:
	VKTexture2D(bsize Width, bsize Height, bsize Mips, bsize Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage, void* data = 0);
	VKTexture2D(bsize Width, bsize Height, BearRenderTargetFormat Format);
	VKTexture2D(bsize Width, bsize Height, BearDepthStencilFormat Format);
	VkImage Image;
	VkDeviceMemory ImageMemory;
	VkImageView ImageView;
	VkDescriptorImageInfo DescriptorImageInfo;
	VkImageCreateInfo ImageInfo;
	virtual ~VKTexture2D();
public:
	virtual void Set(VkWriteDescriptorSet* HEAP);
	virtual BearTextureType GetType();
	virtual void* Lock(bsize mip, bsize depth);
	virtual void Unlock();
private:
	void* m_buffer;
	bsize m_mip;
	bsize m_depth;
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