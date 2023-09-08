/// \file ae_image.hpp
/// The image class is defined.
#pragma once

// dependencies
#include "ae_device.hpp"

// libraries
#include <stb_image.h>

// std
#include <memory>

namespace ae {
    /// Responsible for creating and managing images that will be used for different graphical, or computational,
    /// functions of the engine.
    class AeImage {
    public:
        struct Builder {
            /// The pixel data of the image.
            stbi_uc* pixels;

            /// The width of the image.
            int imageWidth;

            /// The height of the image.
            int imageHeight;

            /// The number of channels of the image.
            int channelQuantity;

            /// The format of the image.
            VkFormat imageFormat;

            /// The overall size of the image. This matters since the number of channels imported for the image will
            /// dictate this value.
            VkDeviceSize imageSize;

            /// Loads an image from the file at the specified path and populates the Builder struct.
            /// \param t_filepath The path to the model file to be loaded.
            void loadImage(const std::string& t_filepath);

            ~Builder();
        };

        /// Construct a new AeImage creating the vulkan image, using a buffer, so the image is ready for the specified
        /// device.
        /// \param t_device The GPU the image maybe loaded onto and needs to be compatible with.
        /// \param t_builder The builder that contains the image information that buffers should be created for ready
        /// for the specified device.
        AeImage(AeDevice &t_device, const AeImage::Builder &t_builder);

        /// Destroy the AeImage object.
        ~AeImage();

        /// Do not allow this class to be copied (2 lines below)
        AeImage(const AeImage&) = delete;
        AeImage& operator=(const AeImage&) = delete;

        /// Creates an AeImage using the specified image data stored at the specified file path compatible with the
        /// specified GPU.
        /// \param t_device The GPU the created image will be compatible with and will have buffer created for.
        /// \param t_filepath The location of the file defining the image.
        static std::unique_ptr<AeImage> createModelFromFile(AeDevice& t_device, const std::string& t_filepath);

    private:


        /// Creates, and allocates memory for, the image that the texture will be loaded into.
        /// \param t_builder The builder class that is being used to construct the image.
        void createImage(const AeImage::Builder &t_builder, VkImageTiling t_tiling, VkImageUsageFlags t_usage, VkMemoryPropertyFlags t_properties);

        /// Transition the layout of the image.
        /// \param t_newLayout The new layout that the image must be transitioned to.
        void transitionLayout(VkImageLayout t_newLayout);

        /// Populates the image with the data from the image's Builder struct.
        /// \param t_builder The builder struct containing the image data to create the buffer from.
        void fillImage(const AeImage::Builder &t_builder);

        /// Creates an image view for the image.
        void createImageView();

        /// The device the image has been created on.
        AeDevice &m_aeDevice;

        /// The image this class handles.
        VkImage m_image = VK_NULL_HANDLE;

        /// The device memory allocated for the image.
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        /// The image view of this image.
        VkImageView m_imageView;

        /// The format of the image.
        VkFormat m_format;

        /// The current layout of the image.
        VkImageLayout m_layout;

    protected:

    };
} // namespace ae