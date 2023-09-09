/// \file ae_samplers.cpp
/// The AeSamplers class is implemented.
#include "ae_samplers.hpp"

// dependencies

// libraries

// std
#include <stdexcept>

namespace ae {

    // Create the required samplers for the specified device.
    AeSamplers::AeSamplers(ae::AeDevice &t_device) : m_aeDevice{t_device} {
        createDefaultSampler();
    };



    // Destroy the samplers.
    AeSamplers::~AeSamplers() {
        vkDestroySampler(m_aeDevice.device(), m_defaultSampler, nullptr);
    }



    // Return the default sampler.
    VkSampler AeSamplers::getDefaultSampler() { return m_defaultSampler;};



    // Create the default sampler.
    void AeSamplers::createDefaultSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        // Select bilinear filtering for the sampling method to help with situation where there are more fragments than
        // pixels.
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        // Repeat the texture when going beyond the specified image dimension, aka. tiling.
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        // Enable anisotropic filtering, set based on maximum device limits.
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = m_aeDevice.m_properties.limits.maxSamplerAnisotropy;

        // Color that is displayed when using clamp mode and going beyond the image's dimensions.
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

        // Use the [0,1] range for texture coordinates aka. normalized.
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        // Texels will first be compared to a value, and the result of that comparison is used in filtering operations.
        // This is mainly used for percentage-closer filtering on shadow maps.
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        // Currently not using mipmapping, so set to defaults.
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        // Attempt to create the sampler.
        if (vkCreateSampler(m_aeDevice.device(), &samplerInfo, nullptr, &m_defaultSampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the default texture sampler!");
        }

    };



} //namespace ae