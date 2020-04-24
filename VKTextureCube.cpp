#include "VKPCH.h"
size_t TextureCubeCounter = 0;
VKTextureCube::VKTextureCube(size_t Width, size_t Height, size_t Mips, size_t Count, BearTexturePixelFormat PixelFormat, BearTextureUsage typeUsage, void* data)
{
    TextureCubeCounter++;
    Format = PixelFormat;
    TextureUsage = typeUsage;
    TextureType = TT_Default;
    {
        ZeroMemory(&ImageInfo,sizeof(VkImageCreateInfo));
        ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageInfo.extent.width =static_cast<uint32_t>( Width);
        ImageInfo.extent.height = static_cast<uint32_t>(Height);
        ImageInfo.extent.depth = 1;;
        ImageInfo.mipLevels = static_cast<uint32_t>(Mips);
        ImageInfo.arrayLayers = static_cast<uint32_t>(Count)*6;
        ImageInfo.format = Factory->Translation(PixelFormat);
        ImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageInfo.usage =TextureUsage==TU_STATING ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT| VK_IMAGE_USAGE_TRANSFER_DST_BIT : VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        ImageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        V_CHK(vkCreateImage(Factory->Device, &ImageInfo, nullptr, &Image));
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(Factory->Device, Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(Factory->PhysicalDevice,memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        V_CHK(vkAllocateMemory(Factory->Device, &allocInfo, nullptr, &ImageMemory));
        vkBindImageMemory(Factory->Device, Image, ImageMemory, 0);
    }

  
   

    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = Image;
        viewInfo.viewType = Count>1? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
        viewInfo.format = Factory->Translation(PixelFormat);
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = static_cast<uint32_t>(Mips);
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = static_cast<uint32_t>(Count);

        V_CHK(vkCreateImageView(Factory->Device, &viewInfo, nullptr, &ImageView));
    }
    TransitionImageLayout(0, Image, ImageInfo.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, ImageInfo.mipLevels,  ImageInfo.arrayLayers,0);
    m_buffer = 0;
    switch (TextureUsage)
    {
    case TU_DYNAMIC:
    case TU_STATING:
        AllocBuffer();
        break;
    }
    if (data)
    {
        auto ptr = reinterpret_cast<uint8*>(data);
        for (size_t x = 0; x < Count*6; x++)
            for (size_t y = 0; y < Mips; y++)
            {
                size_t  size = BearTextureUtils::GetSizeDepth(BearTextureUtils::GetMip(Width, y), BearTextureUtils::GetMip(Height, y), Format);
                memcpy(Lock(y, x), ptr, size);
                Unlock();
                ptr += size;
            }


    }
}

void VKTextureCube::SetAsSRV(VkWriteDescriptorSet* HEAP, size_t offset)
{
    BEAR_CHECK(ImageView);
    DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    DescriptorImageInfo.imageView = ImageView;
    DescriptorImageInfo.sampler = Factory->DefaultSampler;
    HEAP->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    HEAP->pImageInfo = &DescriptorImageInfo;
}

void* VKTextureCube::QueryInterface(int Type)
{
    switch (Type)
    {
    case VKQ_ShaderResource:
        return reinterpret_cast<void*>(static_cast<VKShaderResource*>(this));
    default:
        return nullptr;
    }
}

VKTextureCube::~VKTextureCube()
{
    TextureCubeCounter--;
    if(ImageView)
    vkDestroyImageView(Factory->Device, ImageView, nullptr);
    vkDestroyImage(Factory->Device, Image, nullptr);
    vkFreeMemory(Factory->Device, ImageMemory, nullptr);
    switch (TextureUsage)
    {
    case TU_DYNAMIC:
    case TU_STATING:
        FreeBuffer();
        break;
    }
}


BearTextureType VKTextureCube::GetType()
{
    return TextureType;
}

void* VKTextureCube::Lock(size_t mip, size_t depth)
{
    m_mip = mip;
    m_depth = depth;
    if (TextureType != TT_Default)return 0;
    if (m_buffer)Unlock();
    switch (TextureUsage)
    {
    case TU_STATIC:
        AllocBuffer();
        break;
    case TU_DYNAMIC:
        break;
    case TU_STATING:
        Factory->LockCommandBuffer();
        TransitionImageLayout(Factory->CommandBuffer, Image, Factory->Translation(Format), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ImageInfo.mipLevels, 1, static_cast<uint32_t>(m_depth));
        CopyImageToBuffer(Factory->CommandBuffer, StagingBuffer, Image, ImageInfo.extent.width, ImageInfo.extent.height, static_cast<uint32_t>(m_mip), static_cast<uint32_t>(m_depth));
        TransitionImageLayout(Factory->CommandBuffer, Image, Factory->Translation(Format), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, ImageInfo.mipLevels, 1, static_cast<uint32_t>(m_depth));
        Factory->UnlockCommandBuffer();
        break;
    default:
        break;
    }
    vkMapMemory(Factory->Device, StagingBufferMemory, 0, BearTextureUtils::GetSizeInMemory(BearTextureUtils::GetMip( ImageInfo.extent.width,mip), BearTextureUtils::GetMip(ImageInfo.extent.height, mip), ImageInfo.mipLevels, Format), 0, &m_buffer);

    return m_buffer;
}

void VKTextureCube::Unlock()
{
    vkUnmapMemory(Factory->Device, StagingBufferMemory);
    switch (TextureUsage)
    {
    case TU_STATIC:
        Factory->LockCommandBuffer();
        TransitionImageLayout(Factory->CommandBuffer, Image, Factory->Translation(Format), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, ImageInfo.mipLevels,1, static_cast<uint32_t>(m_depth));
        CopyBufferToImage(Factory->CommandBuffer, StagingBuffer, Image, ImageInfo.extent.width, ImageInfo.extent.height, static_cast<uint32_t>(m_mip), static_cast<uint32_t>(m_depth));
        TransitionImageLayout(Factory->CommandBuffer, Image, Factory->Translation(Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, ImageInfo.mipLevels, 1, static_cast<uint32_t>(m_depth));
        Factory->UnlockCommandBuffer();
        FreeBuffer();
        break;
    case TU_DYNAMIC:
        Factory->LockCommandBuffer();
        TransitionImageLayout(Factory->CommandBuffer, Image, Factory->Translation(Format), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, ImageInfo.mipLevels, 1, static_cast<uint32_t>(m_depth));
        CopyBufferToImage(Factory->CommandBuffer, StagingBuffer, Image, ImageInfo.extent.width, ImageInfo.extent.height, static_cast<uint32_t>(m_mip), static_cast<uint32_t>(m_depth));
        TransitionImageLayout(Factory->CommandBuffer, Image, Factory->Translation(Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, ImageInfo.mipLevels,1, static_cast<uint32_t>(m_depth));
        Factory->UnlockCommandBuffer();
        break;
    case TU_STATING:
        break;
    default:
        break;
    }
    m_buffer = 0;
  
    
   

 
}

void VKTextureCube::AllocBuffer()
{
    VkDeviceSize imageSize = BearTextureUtils::GetSizeInMemory(ImageInfo.extent.width, ImageInfo.extent.height, ImageInfo.mipLevels, Format) *  ImageInfo.arrayLayers;
    CreateBuffer(Factory->PhysicalDevice, Factory->Device, imageSize, TextureUsage == TU_STATING ? VK_BUFFER_USAGE_TRANSFER_DST_BIT : VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);
}

void VKTextureCube::FreeBuffer()
{
    vkDestroyBuffer(Factory->Device, StagingBuffer, nullptr);
    vkFreeMemory(Factory->Device, StagingBufferMemory, nullptr);
}

