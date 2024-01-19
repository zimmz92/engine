/*! \file ae_device.cpp
    \brief The script defining the vulkan device interface class.
    The vulkan device interface class is defined.
*/
#pragma once

#include "ae_graphics_constants.hpp"
#include "ae_window.hpp"

// Libraries
#include <string>
#include <vector>
#include <optional>

namespace ae {

    /// Stores the swap chain capabilities of the GPU.
	struct SwapChainSupportDetails {
        /// The surface capabilities of the GPU.
		VkSurfaceCapabilitiesKHR capabilities;

        /// The surface format of the GPU.
		std::vector<VkSurfaceFormatKHR> formats;

        /// The available presentation modes of the GPU.
		std::vector<VkPresentModeKHR> presentModes;
	};

    /// Structure that stores the desired queue families and their device information.
	struct QueueFamilyIndices {
        /// Index of the graphics queue family on the device.
		uint32_t graphicsFamily = 0;

        /// Index of the present queue family on the device.
		uint32_t presentFamily = 0;

        /// Index of the compute queue family on the device.
        uint32_t computeFamily = 0;

        /// Represents if a graphics queue family has been found.
		bool graphicsFamilyHasValue = false;

        /// Represents if a present queue family has been found.
		bool presentFamilyHasValue = false;

        /// Represents if a compute queue family has been found.
        bool computeFamilyHasValue = true;

        /// Checks to see if all the desire queue families have been found.
        /// \return True if all the required queue families have been found and indexes populated.
		[[nodiscard]] bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue && computeFamilyHasValue; }
	};

    /// A vulkan device interface object.
	class AeDevice {
	public:

		// Enable validation layers if debugging
#ifdef NDEBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif
	
		/// Creates the vulkan device from the best available GPU from the application's perspective.
		/// \param t_window The window the GPU will be interfacing with.
		AeDevice(AeWindow& t_window);

		/// Destroys the vulkan device interface object.
		~AeDevice();

		/// Do not allow this class to be copied (2 lines below)
		AeDevice(const AeDevice&) = delete;
		AeDevice& operator=(const AeDevice&) = delete;

		/// Do not allow this class to be moved (2 lines below)
		AeDevice(AeDevice&&) = delete;
		AeDevice& operator=(AeDevice&&) = delete;

		/// Get the device's command pool.
		/// \return The command pool of the device.
		VkCommandPool getCommandPool() { return m_commandPool; }

        /// Get the vulkan device.
        /// \return The vulkan device this object is the interface for.
		VkDevice device() { return m_device; }

        /// Get the surface the device corresponds with.
        /// \return The surface this device corresponds with.
		VkSurfaceKHR surface() { return m_surface; }

        /// Get the device's graphics queue.
        /// \return The graphics queue of the device.
		VkQueue graphicsQueue() { return m_graphicsQueue; }

        /// Get the device's presentation queue.
        /// \return The presentation queue of the device.
		VkQueue presentQueue() { return m_presentQueue; }

        /// Get the device's compute queue.
        /// \return The compute queue of the device.
        VkQueue computeQueue() { return m_computeQueue; }

		/// Fetches the swap chain features supported by the device
		/// \return A structure containing the device's swap chain capabilities.
		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }

		/// Finds the memory on the device of the desired memory type with the specified properties.
		/// \param t_typeFilter The memory type to be considered.
		/// \param t_properties The memory properties required.
		/// \return The memory index to device memory that meets the specified requirements.
		uint32_t findMemoryType(uint32_t t_typeFilter, VkMemoryPropertyFlags t_properties);

		/// Returns the queue families that meet the criteria for the application.
		/// \return A structure containing the queue family information required for the application.
		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(m_physicalDevice); }

		/// Get a supported image format that meets the specified tiling and feature requirements
		/// \param t_candidates A list of candidate image formats that should be considered, if available.
		/// \param t_tiling The desired tiling arrangement the format needs to support.
		/// \param t_features The desired tiling features the format needs to support.
		/// \return A supported image format supported by the device that meets the specified requirements.
		VkFormat findSupportedFormat(const std::vector<VkFormat>& t_candidates, VkImageTiling t_tiling, VkFormatFeatureFlags t_features);

		/// Creates a command buffer from the device's command buffer pool and allocates memory for it.
		/// \param t_size Specifies the desired size of the buffer to be created.
		/// \param t_usage Specifies how the buffer will be used.
		/// \param t_properties Specifies the memory properties for the memory to be allocated for the buffer.
		/// \param t_buffer A reference that should be populated with the location of the created command buffer.
		/// \param t_bufferMemory A reference that should be populated with the location of the allocated device memory
		/// for the buffer.
		void createBuffer(
			VkDeviceSize t_size,
			VkBufferUsageFlags t_usage,
			VkMemoryPropertyFlags t_properties,
			VkBuffer& t_buffer,
			VkDeviceMemory& t_bufferMemory);

		/// Returns a command buffer that is ready to have commands recorded to it and will only be executed once.
		/// \return A one-shot command buffer that is ready have command recorded to it.
		VkCommandBuffer beginSingleTimeCommands();

		/// Executes and clean up a one-shot command buffer.
		/// \param t_commandBuffer The one-shot command buffer that should be executed and then cleaned up.
		void endSingleTimeCommands(VkCommandBuffer t_commandBuffer);

		/// Copies the "srcBuffer" to the "dstBuffer" using a one-shot command buffer.
		/// \param t_srcBuffer The buffer containing the information to be copied.
		/// \param t_dstBuffer The buffer where the information is to be copied to.
		/// \param t_size The size of the srcBuffer.
		void copyBuffer(VkBuffer t_srcBuffer, VkBuffer t_dstBuffer, VkDeviceSize t_size);

		/// Copy the command buffer data/results to an image object.
		/// \param t_buffer The command buffer the data/results are to be copied from.
		/// \param t_image The image the data/results from the command buffer are to be copied to.
		/// \param t_width The image width.
		/// \param t_height The image height.
		/// \param t_layerCount The number of layers that are to be copied.
		void copyBufferToImage(VkBuffer t_buffer, VkImage t_image, uint32_t t_width, uint32_t t_height, uint32_t t_layerCount);

		/// Creates, and allocates memory for, an image.
		/// \param t_imageInfo The information required to create the desired image.
		/// \param t_properties The memory requirements of the image being created.
		/// \param t_image A reference to the image that will be populated with the location of the created image.
		/// \param t_imageMemory A reference to the image memory that will be populated with the location of the
		/// allocated image memory.
		void createImageWithInfo(
			const VkImageCreateInfo& t_imageInfo,
			VkMemoryPropertyFlags t_properties,
			VkImage& t_image,
			VkDeviceMemory& t_imageMemory);

        /// The properties of the GPU which this device interfaces with.
		VkPhysicalDeviceProperties m_properties;

	private:
		/// Creates the vulkan instance that will initialize the vulkan library and passes the application information
		/// to it.
		void createInstance();

		/// Setup debugging messaging if validation layers are enabled.
		void setupDebugMessenger();

		/// Creates a window surface for instance.
		void createSurface();

		/// Chooses between multiple physical devices based on device features.
		void pickPhysicalDevice();

		/// Create a virtual device based on required queues for the application.
		void createLogicalDevice();

		/// Add required commands from the queue families to the logical device's command pool.
		void createCommandPool();

		/// Rates a GPU based on the required application features.
		int rateDeviceSuitability(VkPhysicalDevice t_device);

		/// Gets the required extensions for GLFW library.
		/// \return The extensions required by the GLFW library.
		std::vector<const char*> getRequiredGLFWExtensions();

		/// Check if the instance, vulkan library, supports the required validation layers.
		bool checkValidationLayerSupport();

		/// Finds the queue families on the GPU that supports the desired features.
		/// \param t_device The GPU that will be searched for the desired queue families.
		/// \return The populated QueueFamilyIndices struct containing the pointers to the required queue families.
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice t_device);

		/// Populates the debug messenger creation information.
		/// \param t_createInfo A reference to the VkDebugUtilsMessengerCreateInfoEXT to be populated with the default
		/// information this function provides.
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& t_createInfo);

		/// Checks if the instance supports the required GLFW extensions.
		/// \param t_requiredExtensions The GLFW extensions required for the application to operate properly.
		void hasGlfwRequiredInstanceExtensions(std::vector<const char*>* t_requiredExtensions);

		/// Ensure device supports all required extensions.
		/// \param t_device The GPU that needs to support the required device extensions.
		bool checkDeviceExtensionSupport(VkPhysicalDevice t_device);

		/// Get the swap chain capabilities of the GPU.
		/// \param t_device The GPU that is to be checked for it's swap chain capabilities.
		/// \return The SwapChainSupportDetails structure containing the swap chain capabilities of the specified GPU.
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice t_device);

		/// The vulkan instance used by this application.
		VkInstance m_instance;

        /// The debugger that will be used by this application if validation layers are enabled.
		VkDebugUtilsMessengerEXT m_debugMessenger;

        /// The GPU being used.
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        /// A reference to the window this device interfaces with.
		AeWindow& m_window;

        /// The command pool available to create command buffers from.
		VkCommandPool m_commandPool;

        /// The vulkan device which provides the interface between the application and the GPU.
		VkDevice m_device;

        /// The vulkan surface that is mapped to the window.
		VkSurfaceKHR m_surface;

        /// The graphics queue family this device uses.
		VkQueue m_graphicsQueue;

        /// The present queue family this device uses.
		VkQueue m_presentQueue;

        /// The compute queue family this device uses.
        VkQueue m_computeQueue;

		/// Required validation Layers
		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		/// Required device extensions
		const std::vector<const char*> m_deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};

} // namespace ae


