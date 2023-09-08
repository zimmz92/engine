/// \file ae_image.cpp
/// The image class is implemented.
#include "ae_image.hpp"

// dependencies
#include "ae_buffer.hpp"

// libraries
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//std
#include <stdexcept>


namespace ae {

    AeImage::AeImage(ae::AeDevice &t_device, const AeImage::Builder &t_builder) : m_aeDevice{t_device}, m_format{t_builder.imageFormat} {

        // Create an image on the GPU for the imported image to be loaded into.
        createImage(t_builder,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Transition the image layout to be optimal for when it is copied to the buffer.
        transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Fill the image with the image information specified in the Builder struct.
        fillImage(t_builder);

        // Transition the image to make it ready for sampling from the shader(s).
        transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Create the view for the texture in order for the shader to access the image.
        createImageView();
    };



    // Clean up the AeImage object resources.
    AeImage::~AeImage(){
        vkDestroyImage(m_aeDevice.device(), m_image, nullptr);
        vkFreeMemory(m_aeDevice.device(), m_memory, nullptr);
    };



    // Create the image from the specified file using the builder class. This
    std::unique_ptr<AeImage> AeImage::createModelFromFile(AeDevice& t_device, const std::string& t_filepath){

        // Create a new builder object to store the temporary data required to build the image from the file.
        Builder builder{};
        builder.loadImage(t_filepath);

        // Create the image using the temporary image data from the import.
        return std::make_unique<AeImage>(t_device, builder);
    };



    // Create, and allocate memory for, the image for the texture to be loaded into.
    void AeImage::createImage(const AeImage::Builder &t_builder, VkImageTiling t_tiling, VkImageUsageFlags t_usage, VkMemoryPropertyFlags t_properties){

        // Specify the properties of the image to be created.
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = t_builder.imageWidth;
        imageInfo.extent.height = t_builder.imageHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = m_format;
        imageInfo.tiling = t_tiling;
        imageInfo.usage = t_usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Set the initial layout of the image as undefined.
        m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.initialLayout = m_layout;

        // Attempt to create the image.
        if (vkCreateImage(m_aeDevice.device(), &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create an image!");
        }

        // Get the memory requirements for the image to be stored on the device.
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_aeDevice.device(), m_image, &memRequirements);

        // Specify what memory should be allocated and how much of it.
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = m_aeDevice.findMemoryType(memRequirements.memoryTypeBits, t_properties);

        // Attempt to allocate device memory for the image.
        if (vkAllocateMemory(m_aeDevice.device(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory for the image!");
        }

        // Bind the device memory allocated for the image to the image.
        vkBindImageMemory(m_aeDevice.device(), m_image, m_memory, 0);
    };


    void AeImage::transitionLayout(VkImageLayout t_newLayout){

        // Use an image memory barrier to transition the layout of the image.
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = m_layout;
        barrier.newLayout = t_newLayout;

        // Check the layout transitions supported by this function and setup flags for the transition accordingly.
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
        if (m_layout == VK_IMAGE_LAYOUT_UNDEFINED && t_newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            // Since this is just transferring data from a buffer to the image there is no need for the pipeline to wait
            // on anything.
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (m_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && t_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            // Shader reads need to wait on transfer writes, specifically the shader reads in the fragment shader,
            // because that is where this image is most likely going to be used.
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("Unsupported layout transition requested!");
        }

        // Currently not transferring queue families during this operation.
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        // Currently no mipmapping so levels and layers are trivial.
        barrier.image = m_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        // TODO: Specify which types of operations need to finish before this operation occurs. This will depend on the
        //  types of transitions that will be performed.
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        // Start a command buffer one shot command
        VkCommandBuffer commandBuffer = m_aeDevice.beginSingleTimeCommands();

        // Add the transition command to the one-shot command to be executed.
        vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );

        // Execute the transition
        m_aeDevice.endSingleTimeCommands(commandBuffer);

        // Save the layout that was transitioned to.
        m_layout = t_newLayout;
    };



    // Make an image buffer for the image being built.
    void AeImage::fillImage(const AeImage::Builder &t_builder){
        AeBuffer stagingBuffer{
                m_aeDevice,
                t_builder.imageSize,
                1,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        // Map the memory for the staging buffer.
        stagingBuffer.map();

        // Write the data to the staging buffer.
        stagingBuffer.writeToBuffer(t_builder.pixels);

        // Copy the data from the staging buffer to the image.
        m_aeDevice.copyBufferToImage(stagingBuffer.getBuffer(),
                                     m_image,
                                     static_cast<uint32_t>(t_builder.imageWidth),
                                     static_cast<uint32_t>(t_builder.imageHeight),
                                     1);
    };


    //==================================================================================================================
    // Builder
    //==================================================================================================================

    // Load the image information from the specified file.
    void AeImage::Builder::loadImage(const std::string &t_filepath) {

        // Load the image from the file forcing the image to load with an alpha channel even if it does not exist
        // natively in the image being imported. Set the image format to align the texels to the import properties of
        // the pixels.
        pixels = stbi_load(t_filepath.c_str(), &imageWidth, &imageHeight, &channelQuantity, STBI_rgb_alpha);
        imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

        // Check to make sure that the image was actually loaded from the file.
        if (!pixels){
            std::string errorMessage = std::string("Could not load texture from the file: ") +
                                       t_filepath +
                                       std::string(" !");
            throw std::runtime_error(errorMessage);
        }

        // The size of the imported image is based on the number of channels imported. In the case of STBI_rgb_alpha the
        // channels R, G, B, and alpha are imported therefore making size of the image the number of pixels multiplied
        // by the number of channels imported, 4.
        imageSize = imageWidth * imageHeight * (channelQuantity+1);

    };


    void AeImage::createImageView() {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.components = VK_COMPONENT_SWIZZLE_IDENTITY;

        if (vkCreateImageView(m_aeDevice.device(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the image view for an image!");
        }
    };



    // Make sure to clean up the loaded pixel data when the structure is destroyed.
    AeImage::Builder::~Builder(){
        stbi_image_free(pixels);
    };

} //namespace ae