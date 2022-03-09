#include "ae_device.hpp"

// Libraries
#include <iostream>
#include <cstring>
#include <set>
#include <unordered_set>
#include <map>

namespace ae {

	// Local callback functions

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT t_messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT t_messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* t_pCallbackData,
		void* t_pUserData) {

		std::cerr << "validation layer: " << t_pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance t_instance,
		const VkDebugUtilsMessengerCreateInfoEXT* t_createInfo,
		const VkAllocationCallbacks* t_allocator,
		VkDebugUtilsMessengerEXT* t_debugMessenger) {

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(t_instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(t_instance, t_createInfo, t_allocator, t_debugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessenerEXT(
		VkInstance t_instance,
		VkDebugUtilsMessengerEXT t_debugMessenger,
		const VkAllocationCallbacks* t_allocator) {

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(t_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(t_instance, t_debugMessenger, t_allocator);
		}
	}
	
	// Function to create graphics device
	AeDevice::AeDevice(AeWindow& t_window) : m_window{ t_window } {
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createCommandPool();
	}

	// Function to destroy graphics device
	AeDevice::~AeDevice() {
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
		vkDestroyDevice(m_device, nullptr);

		if (m_enableValidationLayers) {
			DestroyDebugUtilsMessenerEXT(m_instance, m_debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);
	}

	// Function to create the Vulkan instance
	void AeDevice::createInstance() {
		if (m_enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested but are not available!");
		}

		// Base information structure for Vulkan Application
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Arundos Engine Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Extensions required by glfw to build the window
		auto requiredExtensions = getRequiredExtentions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		// Include validation layers if enabled
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (m_enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// Check if extentions required by GFLW are supported
		hasGflwRequiredInstanceExtensions(&requiredExtensions);

		// Create Vulkan Instance
		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	// Function to choose between multiple physical devies based on available device features
	void AeDevice::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		std::cout << "Device count: " << deviceCount << std::endl;
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		// Use an ordered map to auto sort candidates for default graphics card selection by increasing score
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices) {
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Ensure choosen device is actually suitable
		if (candidates.rbegin()->first > 0) {
			m_physicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("Failed to find sutable GPU!");
		}

		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
		std::cout << "physical device: " << m_properties.deviceName << std::endl;
	}

	// Function to create a virtual device based on required queues
	void AeDevice::createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		// TODO: LittleVulkanEngine tutorial adds the line below.... why?
		//deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

		if (m_enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logicial device");
		}

		vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_presentQueue);
	}

	// Funciton to add required commands from the queue families to the device command pool
	void AeDevice::createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = 0;
		// TODO: LittleVulkanEngine tutorial flags defined as below.... why?
		//poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create command pool!");
		}
	}

	// Function to create a window surface for an instance
	void AeDevice::createSurface() { m_window.createWindowSurface(m_instance, &m_surface); }

	// Function to populate the debug messeger information
	void AeDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& t_createInfo) {
		t_createInfo = {};
		t_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		t_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		t_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		t_createInfo.pfnUserCallback = debugCallback;
		t_createInfo.pUserData = nullptr; // Optional
	}

	// Function to setup debugging messaging if validation layers are enabled
	void AeDevice::setupDebugMessenger() {
		if (!m_enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	// Function to check if the instance supports the required validation layers
	bool AeDevice::checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	// Function to rate a device based on required applicaiton features
	int AeDevice::rateDeviceSuitability(VkPhysicalDevice t_device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(t_device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(t_device, &deviceFeatures);

		QueueFamilyIndices indices = findQueueFamilies(t_device);

		int score = 0;

		// Prefer discrete GPU over internal
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// Prefer devices that have greater maximum texture sizes
		score += deviceProperties.limits.maxImageDimension2D;

		// Ensure device has required geometry shaders, queue families, and device extentions
		if (!deviceFeatures.geometryShader || !indices.isComplete() || !checkDeviceExtensionSupport(t_device)) {
			return 0;
		}
		else {
			// Check if required swap chain support is available
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(t_device);
			if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
				return 0;
			}
		}

		return score;
	}

	// Function to return the required extentions for glfw
	std::vector<const char*> AeDevice::getRequiredExtentions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	// Function that returns the extentions required to create a glfw instance
	void AeDevice::hasGflwRequiredInstanceExtensions(std::vector<const char*> *t_requiredExtensions) {
		// Check if required extensions are supported by the hardware
		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

		bool extsUnavail = false;

		for (const auto& requiredExtension : *t_requiredExtensions) {
			bool extUnavail = true;
			for (const auto& availableExtention : availableExtensions) {
				if (strcmp(requiredExtension, availableExtention.extensionName) == 0) {
					extUnavail = false;
				}
			}
			if (extUnavail) {
				std::cout << "REQUIRED EXTENTION " << requiredExtension << " UNAVAILABLE!" << '\n';
				extsUnavail = true;
			}
		}

		if (extsUnavail) {
			throw std::runtime_error("One or more required extentions are unavailable!");
		}
	}

	// Function to ensure device supports all required extentions
	bool AeDevice::checkDeviceExtensionSupport(VkPhysicalDevice t_device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	// Find a queue family that supports all the desired features
	QueueFamilyIndices AeDevice::findQueueFamilies(VkPhysicalDevice t_device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queueFamilyCount, queueFamilies.data());


		// Pick a queue family that supports both drawing and presentation in same queue for improved performance
		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
			indices.graphicsFamilyHasValue = true;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(t_device, i, m_surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
			indices.presentFamilyHasValue = true;
		}
		if (indices.isComplete()) {
			break;
		}

		i++;
		}

		return indices;
	}

	// Function to requrn the properties of the swap chain
	SwapChainSupportDetails AeDevice::querySwapChainSupport(VkPhysicalDevice t_device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t_device, m_surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, m_surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, m_surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, m_surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	// Function to return the candidate formats that meet the tiling and feature requirements
	VkFormat AeDevice::findSupportedFormat(
		const std::vector<VkFormat>& t_candidates, VkImageTiling t_tiling, VkFormatFeatureFlags t_features) {
		for (VkFormat format : t_candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

			if (t_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & t_features) == t_features) {
				return format;
			}
			else if (
				t_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & t_features) == t_features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	// Function to check if the device has memory of a specific type with specific properties
	uint32_t AeDevice::findMemoryType(uint32_t t_typeFilter, VkMemoryPropertyFlags t_properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((t_typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & t_properties) == t_properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	// Function to create a command buffer and allocate memory for it
	void AeDevice::createBuffer(
		VkDeviceSize t_size,
		VkBufferUsageFlags t_usage,
		VkMemoryPropertyFlags t_properties,
		VkBuffer& t_buffer,
		VkDeviceMemory& t_bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = t_size;
		bufferInfo.usage = t_usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &t_buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device, t_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, t_properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &t_bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_device, t_buffer, t_bufferMemory, 0);
	}

	// Function to start running the command buffer once
	VkCommandBuffer AeDevice::beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	// Function to end and clear the singular command buffer run
	// TODO: implement memory barrier to improve performance
	void AeDevice::endSingleTimeCommands(VkCommandBuffer t_commandBuffer) {
		vkEndCommandBuffer(t_commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &t_commandBuffer;

		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicsQueue);

		vkFreeCommandBuffers(m_device, m_commandPool, 1, &t_commandBuffer);
	}

	// Function to copy a command buffer
	void AeDevice::copyBuffer(VkBuffer t_srcBuffer, VkBuffer t_dstBuffer, VkDeviceSize t_size) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = t_size;
		vkCmdCopyBuffer(commandBuffer, t_srcBuffer, t_dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	// Funciton to copy the command buffer results to an immage
	void AeDevice::copyBufferToImage(
		VkBuffer t_buffer, VkImage t_image, uint32_t t_width, uint32_t t_height, uint32_t t_layerCount) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = t_layerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { t_width, t_height, 1 };

		vkCmdCopyBufferToImage(
			commandBuffer,
			t_buffer,
			t_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);
		endSingleTimeCommands(commandBuffer);
	}

	// Funciton to create, and allocate memory for, an image
	void AeDevice::createImageWithInfo(
		const VkImageCreateInfo& t_imageInfo,
		VkMemoryPropertyFlags t_properties,
		VkImage& t_image,
		VkDeviceMemory& t_imageMemory) {
		if (vkCreateImage(m_device, &t_imageInfo, nullptr, &t_image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, t_image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, t_properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &t_imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		if (vkBindImageMemory(m_device, t_image, t_imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to bind image memory!");
		}
	}


} // namespace ae
