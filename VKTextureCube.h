#pragma once
class VKTextureCube : public VKShaderResource, public BearRHI::BearRHITextureCube
{
public:
	VKTextureCube(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage TypeUsage = TU_STATIC, void* data = 0);

	VkImage Image;
	VkDeviceMemory ImageMemory;
	VkImageView ImageView;
	VkDescriptorImageInfo DescriptorImageInfo;
	VkImageCreateInfo ImageInfo;
	virtual ~VKTextureCube();
public:
	virtual void SetAsSRV(VkWriteDescriptorSet* HEAP, size_t offset);
	virtual void* QueryInterface(int Type);
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
private:
	void AllocBuffer();
	void FreeBuffer();
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
};