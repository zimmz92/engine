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
#ifdef NDEBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif
	
		AeDevice(AeWindow& t_window);
		~AeDevice();

		// Do not allow this class to be copied (2 lines below)
		AeDevice(const AeDevice&) = delete;
		AeDevice& operator=(const AeDevice&) = delete;

		// Do not allow this class to be moved (2 lines below)
		AeDevice(AeDevice&&) = delete;
		AeDevice& operator=(AeDevice&&) = delete;

		
		VkCommandPool getCommandPool() { return m_commandPool; }
		VkDevice device() { return m_device; }
		VkSurfaceKHR surface() { return m_surface; }
		VkQueue graphicsQueue() { return m_graphicsQueue; }
		VkQueue presentQueue() { return m_presentQueue; }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }
		uint32_t findMemoryType(uint32_t t_typeFilter, VkMemoryPropertyFlags t_properties);

		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(m_physicalDevice); }
		VkFormat findSupportedFormat(const std::vector<VkFormat>& t_candidates, VkImageTiling t_tiling, VkFormatFeatureFlags t_features);

		// Buffer Helper Functions
		void createBuffer(
			VkDeviceSize t_size,
			VkBufferUsageFlags t_usage,
			VkMemoryPropertyFlags t_properties,
			VkBuffer& t_buffer,
			VkDeviceMemory& t_bufferMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer t_commandBuffer);
		void copyBuffer(VkBuffer t_srcBuffer, VkBuffer t_dstBuffer, VkDeviceSize t_size);
		void copyBufferToImage(VkBuffer t_buffer, VkImage t_image, uint32_t t_width, uint32_t t_height, uint32_t t_layerCount);

		void createImageWithInfo(
			const VkImageCreateInfo& t_imageInfo,
			VkMemoryPropertyFlags t_properties,
			VkImage& t_image,
			VkDeviceMemory& t_imageMemory);

		VkPhysicalDeviceProperties m_properties;

	private:
		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createCommandPool();

		// helper functions
		int rateDeviceSuitability(VkPhysicalDevice t_device);
		std::vector<const char*> getRequiredExtentions();
		bool checkValidationLayerSupport();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice t_device);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& t_createInfo);
		void hasGflwRequiredInstanceExtensions(std::vector<const char*>* t_requiredExtensions);
		bool checkDeviceExtensionSupport(VkPhysicalDevice t_device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice t_device);

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


