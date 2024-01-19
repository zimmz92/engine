/*! \file ae_device.hpp
    \brief The script implementing the vulkan device interface class.
    The vulkan device interface class is implemented.
*/
#include "ae_device.hpp"

// Libraries
#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include <string>

namespace ae {

	// Call back for the vulkan validation layer.
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT t_messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT t_messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* t_pCallbackData,
		void* t_pUserData) {

        // Push the validation layer error message to the standard output.
		std::cerr << "validation layer: " << t_pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance t_instance,
		const VkDebugUtilsMessengerCreateInfoEXT* t_createInfo,
		const VkAllocationCallbacks* t_allocator,
		VkDebugUtilsMessengerEXT* t_debugMessenger) {

        // Attempt to create the debug messenger.
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

        // Attempt to destroy the debug messenger.
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

	// Initializes the vulkan library and create a connection between our app and vulkan.
	void AeDevice::createInstance() {

        // Check to see if all the requested validation layers are available.
		if (m_enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested but are not available!");
		}

		// Base information structure for Vulkan Application
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Arundos Engine Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Arundos Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

        // Base information for the instance to be created, with the app information.
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Get the extensions required by glfw to build the window.
		auto requiredExtensions = getRequiredGLFWExtensions();

        // Check if extensions required by GLFW are supported.
        hasGlfwRequiredInstanceExtensions(&requiredExtensions);

        // Pass the GLFW required extensions into the instance configuration struct.
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


		// Create Vulkan Instance
		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	// Function to choose between multiple physical devices based on available device features
	void AeDevice::pickPhysicalDevice() {

        // Get the number of graphics devices available on the current machine.
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

        // Ensure that there actually is at least one graphics device.
		if (deviceCount == 0) {
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

        // Log the number of available devices
		std::cout << "Device count: " << deviceCount << std::endl;

        // Get a list of the available devices.
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		// Use an ordered map to auto sort candidates for default graphics card selection by increasing score
		std::multimap<uint32_t , VkPhysicalDevice> candidates;

        // Rate each device's capabilities then insert them into the ordered map.
		for (const auto& device : devices) {
			uint32_t score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
            VkPhysicalDeviceProperties  localProperties;
            vkGetPhysicalDeviceProperties(device, &localProperties);
#ifdef MY_DEBUG
            std::cout << "physical device: " << localProperties.deviceName << ", score: " << score << std::endl;
#endif
		}

		// Ensure chosen device is actually suitable and is not just the first amount a list of incompatible devices.
		if (candidates.rbegin()->first > 0) {
			m_physicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("Failed to find a suitable GPU!");
		}

        // Retrieve the chosen GPU's properties.
		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
#ifdef MY_DEBUG
        std::cout << "physical device: " << m_properties.deviceName << std::endl;
#endif
	}

	// Function to create a virtual device based on required queues
	void AeDevice::createLogicalDevice() {

        // Get the queue families from the GPU that support our application's needs.
		QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        // Put the queue families into the queue creation information struct.
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

        // Set anisotropy, this helps with tiled texture rendering clarity.
		VkPhysicalDeviceFeatures deviceFeatures{};
        // TODO: Make this optional
		deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.multiDrawIndirect = VK_TRUE;
        deviceFeatures.drawIndirectFirstInstance = VK_TRUE;

        // Create the device creation information.
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Add the queue information for the device being created.
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

        // Add the required device extensions.
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

        VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters{};
        shader_draw_parameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        shader_draw_parameters.pNext = nullptr;
        shader_draw_parameters.shaderDrawParameters = VK_TRUE;

        createInfo.pNext = &shader_draw_parameters;


        // Add the validation layers if enabled.
		if (m_enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

        // Attempt to create the device.
		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device");
		}

        // Get pointers to the created device queues.
		vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_presentQueue);
        vkGetDeviceQueue(m_device, indices.computeFamily, 0, &m_computeQueue);
	}

	// Add required commands from the queue families to the device command pool.
	void AeDevice::createCommandPool() {

        // Get the queue families for the GPU.
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

        // Collect the information for the graphics queue family and add it to the device's command pool.
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        // Attempt to create the command pool.
		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create command pool!");
		}
	}

	// Creates a surface for the vulkan instance and binds it to the GLFW window.
	void AeDevice::createSurface() { m_window.createWindowSurface(m_instance, &m_surface); }

	// Function to populate the debug messenger information
	void AeDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& t_createInfo) {
		t_createInfo = {};
		t_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		t_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		t_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        // Specify the debug callback function to use when an error is caught.
		t_createInfo.pfnUserCallback = debugCallback;
		t_createInfo.pUserData = nullptr; // Optional
	}

	// Setup debugging messaging if validation layers are enabled.
	void AeDevice::setupDebugMessenger() {
        // Only run if validation layers are enabled.
		if (!m_enableValidationLayers) return;

        // Create and populate the configuration file for the debug messenger.
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

        // Attempt to create the debug messenger for the
		if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	// Function to check if the instance supports the required validation layers
	bool AeDevice::checkValidationLayerSupport() {

        // Query the vulkan library to obtain the number of validation layers that are available.
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Retrieve a list of the available validation layers.
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // Loop through the requested validation layers and ensure that they were in the list of available validation
        // layers.
		for (const char* layerName : m_validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
                // Compare the requested validation layer to the specific validation layer in the available list.
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

            // If even one layer is not supported then we cannot build the instance correctly.
			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	// Function to rate a device based on required application features
	int AeDevice::rateDeviceSuitability(VkPhysicalDevice t_device) {
        // Get the properties of the device
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(t_device, &deviceProperties);

        // Get the features of the device.
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(t_device, &deviceFeatures);

        // Get the number of available queue families.
		QueueFamilyIndices indices = findQueueFamilies(t_device);

        // Initialize the score that will be returned.
		uint32_t score = 0;

        // Prefer devices that have greater maximum texture sizes.
        // Make sure on linux it is not using llvmpipe since it returns a very high number for this but performs
        // horribly
        std::string deviceName = deviceProperties.deviceName;
        if(deviceName.find("llvmpipe") == std::string::npos){
            score += deviceProperties.limits.maxImageDimension2D;
        }

		// Heavily prefer discrete GPU to internal GPU.
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// Ensure device has required geometry shaders, queue families, and device extensions for this application.
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

	// Function to return the required extensions for glfw
	std::vector<const char*> AeDevice::getRequiredGLFWExtensions() {

        // Get the required extensions for GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // Create a vector of the extensions.
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_enableValidationLayers) {
            // If validation layers are enabled ensure to enable
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	// Returns the extensions required to create a glfw instance
	void AeDevice::hasGlfwRequiredInstanceExtensions(std::vector<const char*> *t_requiredExtensions) {
		// Get the number of instance extensions available from the vulkan library.
		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

        // Get the list of available instance extensions from the vulkan library.
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

		bool extsUnavail = false;

        // Compare the required extensions to the list of available extensions to ensure they are available.
		for (const auto& requiredExtension : *t_requiredExtensions) {
			bool extUnavail = true;

            // Compare the required extension to each of the extensions in the list of available extensions.
			for (const auto& availableExtention : availableExtensions) {
				if (strcmp(requiredExtension, availableExtention.extensionName) == 0) {
					extUnavail = false;
				}
			}
			if (extUnavail) {
				std::cout << "REQUIRED EXTENSION " << requiredExtension << " UNAVAILABLE!" << '\n';
				extsUnavail = true;
			}
		}

		if (extsUnavail) {
			throw std::runtime_error("One or more required extensions are unavailable!");
		}
	}

	// Function to ensure device supports all required extensions
	bool AeDevice::checkDeviceExtensionSupport(VkPhysicalDevice t_device) {
        // Get the number of available extensions for the GPU
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extensionCount, nullptr);

        // Get a list of available extensions for the GPU
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(t_device, nullptr, &extensionCount, availableExtensions.data());

        // Create a list of the required extensions.
		std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
            // If the extension is one of the required extensions remove it from the list.
			requiredExtensions.erase(extension.extensionName);
		}

        // If the list is empty then we have all the required extension available.
		return requiredExtensions.empty();
	}

	// Find a queue family that supports all the desired features
	QueueFamilyIndices AeDevice::findQueueFamilies(VkPhysicalDevice t_device) {

        // Make a new queue family indices struct.
		QueueFamilyIndices indices;

        // Get the number of available queue families on the graphics device.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queueFamilyCount, nullptr);

        // Get a list of the available queue families on the graphics device.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(t_device, &queueFamilyCount, queueFamilies.data());


		// Pick a queue family that supports both drawing and presentation in same queue for improved performance
		int i = 0;
		for (const auto &queueFamily : queueFamilies) {

            // Check if the current queue family has drawing support
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }

            // Check that the current queue supports presentation to the surface of our instance assigned to our window.
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(t_device, i, m_surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeFamily = i;
                indices.graphicsFamilyHasValue = true;
            }

            // Once a queue family is found that supports drawing and presentation we no longer need to search.
            if (indices.isComplete() && (indices.presentFamily == indices.graphicsFamily == indices.computeFamily)) {
                break;
            }

            i++;
		}

        if(indices.graphicsFamilyHasValue && indices.presentFamilyHasValue){
            return indices;
        } else {
            throw std::runtime_error("Unable to find indices for all the required queue families!");
        };
	}

	// Return the device's swap chain capabilities.
	SwapChainSupportDetails AeDevice::querySwapChainSupport(VkPhysicalDevice t_device) {

        // Create a struct to store the details of the device's swap chain capabilities.
		SwapChainSupportDetails details;

        // Get the basic capabilities of a surface, needed in order to create a swapchain,
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t_device, m_surface, &details.capabilities);

        // Get the number of the supported swapchain format-color space pairs for the surface
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, m_surface, &formatCount, nullptr);

        //Get the supported swapchain format-color space pairs for the surface.
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(t_device, m_surface, &formatCount, details.formats.data());
		}

        // Get the number of supported presentation modes for a surface.
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, m_surface, &presentModeCount, nullptr);

        // Get the supported presentation modes for a surface.
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(t_device, m_surface, &presentModeCount, details.presentModes.data());
		}

        // Return the device's swap chain capability details.
		return details;
	}

	// Get the candidate image formats that meet the tiling and feature requirements
	VkFormat AeDevice::findSupportedFormat(
		const std::vector<VkFormat>& t_candidates, VkImageTiling t_tiling, VkFormatFeatureFlags t_features) {


		for (VkFormat format : t_candidates) {

            // Get the graphics device's format properties.
			VkFormatProperties props;
            //props.linearTilingFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

            // A linear tiling option
			if (t_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & t_features) == t_features) {
				return format;
			}
            // An "optimal" tiling option.
			else if (
				t_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & t_features) == t_features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	// Function to check if the device has memory of a specific type with specific properties
	uint32_t AeDevice::findMemoryType(uint32_t t_typeFilter, VkMemoryPropertyFlags t_properties) {

        // Get the GPU's memory properties.
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

        // Find the memory type that has the required memory property flags.
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

        // Create the base buffer information struct.
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = t_size;
		bufferInfo.usage = t_usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Attempt to create the buffer.
		if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &t_buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

        // Get the memory requirements for the created buffer.
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device, t_buffer, &memRequirements);

        // Create a memory allocation configuration struct.
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;

        // Find the index corresponding to the device memory type that meets the buffer's memory allocation
        // requirements.
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, t_properties);

        // Attempt to allocate memory for the buffer.
		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &t_bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

        // Attach the allocated memory to the buffer object.
		vkBindBufferMemory(m_device, t_buffer, t_bufferMemory, 0);
	}

	// Start a one-shot the command buffer.
	VkCommandBuffer AeDevice::beginSingleTimeCommands() {

        // Create the struct to specify how to allocate the command buffer.
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

        // Allocate a command buffer from the command buffer pool.
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

        // Specify how the command buffer shall be used and be recorded.
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // Begin allowing recording to the command buffer.
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	// End and clear a one-shot command buffer.
	// TODO: implement memory barrier to improve performance
	void AeDevice::endSingleTimeCommands(VkCommandBuffer t_commandBuffer) {

        // Stop recording to the command buffer
		vkEndCommandBuffer(t_commandBuffer);

        // Specify the command buffer information for submitting it to the queue.
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &t_commandBuffer;

        // Submit the command buffer commands to the graphics queue.
		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        // Wait for the commands to be executed.
		vkQueueWaitIdle(m_graphicsQueue);

        // Free the command buffer back to the pool.
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &t_commandBuffer);
	}

	// Copies a command buffer.
    // TODO: Relies on single time command that depends on the device to be idle to ensure that the copy command is
    //  finished before rendering begins. This can be avoided with a memory barrier... implement one!
	void AeDevice::copyBuffer(VkBuffer t_srcBuffer, VkBuffer t_dstBuffer, VkDeviceSize t_size) {

        // Get a new one-shot command buffer from the pool to execute the copy command.
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        // Specify some information about the formatting of the data to be copied and where it should be copied to.
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = t_size;

        // Submit the copy command to the command buffer.
		vkCmdCopyBuffer(commandBuffer, t_srcBuffer, t_dstBuffer, 1, &copyRegion);

        // Execute the copy command.
		endSingleTimeCommands(commandBuffer);
	}

	// Copy the data from the command buffer to an image object.
	void AeDevice::copyBufferToImage(
		VkBuffer t_buffer, VkImage t_image, uint32_t t_width, uint32_t t_height, uint32_t t_layerCount) {

        // Create a new command buffer to facilitate the copy.
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        // Specify how the buffer information should map to the image.
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

        // Submit the copy buffer to image command to the command buffer.
		vkCmdCopyBufferToImage(
			commandBuffer,
			t_buffer,
			t_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

        // Actually execute the copy.
		endSingleTimeCommands(commandBuffer);
	}

	// Create, and allocate memory for, an image
	void AeDevice::createImageWithInfo(
		const VkImageCreateInfo& t_imageInfo,
		VkMemoryPropertyFlags t_properties,
		VkImage& t_image,
		VkDeviceMemory& t_imageMemory) {

        // Attempt to create a new image object with the provided information.
		if (vkCreateImage(m_device, &t_imageInfo, nullptr, &t_image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

        // Get the memory requirements for image based on the desired properties of the image.
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, t_image, &memRequirements);

        // Specified required image information to allocate memory for the image data.
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, t_properties);

        // Attempt to allocate device memory for the image data.
		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &t_imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

        // Attempt to bind the allocated memory to the image object for image data storage.
		if (vkBindImageMemory(m_device, t_image, t_imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to bind image memory!");
		}
	}


} // namespace ae
