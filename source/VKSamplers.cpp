#include "VKPCH.h"
bsize SamplerCounter = 0;
VKSamplerState::VKSamplerState(const BearSamplerDescription& Description)
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerInfo.addressModeU = VKFactory::Translation(Description.AddressU);
	samplerInfo.addressModeV = VKFactory::Translation(Description.AddressV);
	samplerInfo.addressModeW = VKFactory::Translation(Description.AddressW);

	switch (Description.Filter)
	{
	case	SF_MAG_MIP_POINT:
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case	SF_MAG_LINEAR_MIP_POINT:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case	SF_MAG_MIP_LINEAR:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case	SF_ANISOTROPIC:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case	SF_COMPARISON_MAG_MIP_POINT:
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_TRUE;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case	SF_COMPARISON_MAG_LINEAR_MIP_POINT:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_TRUE;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case	SF_COMPARISON_MAG_MIP_LINEAR:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_TRUE;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case	SF_COMPARISON_ANISOTROPIC:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.compareEnable = VK_TRUE;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	default:
		BEAR_ASSERT(0);
	}
	samplerInfo.mipLodBias = static_cast<float>(Description.MipBias);
	samplerInfo.maxLod = 3.402823466e+38f;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.maxAnisotropy =static_cast<float>( Description.MaxAnisotropy);
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    SamplerCounter++;
    V_CHK(vkCreateSampler(Factory->Device, &samplerInfo, nullptr, &ImageInfo.sampler));
}

VKSamplerState::~VKSamplerState()
{
    SamplerCounter--;
	vkDestroySampler(Factory->Device, ImageInfo.sampler, nullptr);
}
