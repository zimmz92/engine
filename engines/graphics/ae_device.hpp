#pragma once

#include "ae_window.hpp"

// Libraries
#include <string>
#include <vector>
#include <optional>

namespace ae {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class AeDevice {
	public:
		// Enable validation layers if debugging
#ifdef NDEBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif
	
		// Function to create graphics device
		AeDevice(AeWindow& t_window);

		// Function to destroy graphics device
		~AeDevice();

		// Do not allow this class to be copied (2 lines below)
		AeDevice(const AeDevice&) = delete;
		AeDevice& operator=(const AeDevice&) = delete;

		// Do not allow this class to be moved (2 lines below)
		AeDevice(AeDevice&&) = delete;
		AeDevice& operator=(AeDevice&&) = delete;

		// Functions to return member variable values
		VkCommandPool getCommandPool() { return m_commandPool; }
		VkDevice device() { return m_device; }
		VkSurfaceKHR surface() { return m_surface; }
		VkQueue graphicsQueue() { return m_graphicsQueue; }
		VkQueue presentQueue() { return m_presentQueue; }

		// Funciton to fetch the swap chain features supported by the device
		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }

		// Function to check if the device has memory of a specific type with specific properties
		uint32_t findMemoryType(uint32_t t_typeFilter, VkMemoryPropertyFlags t_properties);

		// Function to retrun the queue families that meet the criteria for the program
		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(m_physicalDevice); }

		// Function to return the candidate formats that meet the tiling and feature requirements
		VkFormat findSupportedFormat(const std::vector<VkFormat>& t_candidates, VkImageTiling t_tiling, VkFormatFeatureFlags t_features);

		// Function to create a command buffer and allocate memory for it
		void createBuffer(
			VkDeviceSize t_size,
			VkBufferUsageFlags t_usage,
			VkMemoryPropertyFlags t_properties,
			VkBuffer& t_buffer,
			VkDeviceMemory& t_bufferMemory);

		// Function to start running the command buffer once
		VkCommandBuffer beginSingleTimeCommands();

		// Function to end and clear the singular command buffer run
		void endSingleTimeCommands(VkCommandBuffer t_commandBuffer);

		// Function to copy a command buffer
		void copyBuffer(VkBuffer t_srcBuffer, VkBuffer t_dstBuffer, VkDeviceSize t_size);

		// Funciton to copy the command buffer results to an immage
		void copyBufferToImage(VkBuffer t_buffer, VkImage t_image, uint32_t t_width, uint32_t t_height, uint32_t t_layerCount);

		// Funciton to create, and allocate memory for, an image
		void createImageWithInfo(
			const VkImageCreateInfo& t_imageInfo,
			VkMemoryPropertyFlags t_properties,
			VkImage& t_image,
			VkDeviceMemory& t_imageMemory);

		VkPhysicalDeviceProperties m_properties;

	private:
		// Function to create the Vulkan instance
		void createInstance();

		// Function to setup debugging messaging if validation layers are enabled
		void setupDebugMessenger();

		// Function to create a window surface for an instance
		void createSurface();

		// Function to choose between multiple physical devies based on available device features
		void pickPhysicalDevice();

		// Function to create a virtual device based on required queues
		void createLogicalDevice();

		// Funciton to add required commands from the queue families to the device command pool
		void createCommandPool();

		// Function to rate a device based on required applicaiton features
		int rateDeviceSuitability(VkPhysicalDevice t_device);

		// Function to return the required extentions for glfw
		std::vector<const char*> getRequiredGLFWExtensions();

		// Function to check if the instance supports the required validation layers
		bool checkValidationLayerSupport();

		// Find a queue family that supports all the desired features
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice t_device);

		// Function to populate the debug messeger information
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& t_createInfo);

		// Function that returns the extentions required to create a glfw instance
		void hasGlfwRequiredInstanceExtensions(std::vector<const char*>* t_requiredExtensions);

		// Function to ensure device supports all required extentions
		bool checkDeviceExtensionSupport(VkPhysicalDevice t_device);

		// Function to requrn the properties of the swap chain
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice t_device);

		// Member variables
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		AeWindow& m_window;
		VkCommandPool m_commandPool;
		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		// Validation Layers
		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		// Swapchain extension layers
		const std::vector<const char*> m_deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};

} // namespace ae


