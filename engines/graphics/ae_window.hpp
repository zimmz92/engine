#pragma once

#define GLFW_INCLUDE_VULKAN

// Libraries
#include <GLFW/glfw3.h>
#include<string>

namespace ae {

	// Class is the application window
	class AeWindow {
	public:
		// Function to create a glfw window
		AeWindow(int t_w, int t_h, std::string t_name);

		// Function to destroy a glfw window
		~AeWindow();

		// Do not allow this class to be copied
		AeWindow(const AeWindow&) = delete;
		AeWindow& operator=(const AeWindow&) = delete;

		// Do not allow this class to be moved
		AeWindow(AeWindow&&) = delete;
		AeWindow& operator=(AeWindow&&) = delete;

		// Function to check if the window wants to close
		bool shouldClose() { return glfwWindowShouldClose(m_window); }

		// Function to return the size of the window in a Vulkan understanable format
		VkExtent2D getExtent() { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }

		// Function to add the Vulkan surface to the GLFW window
		void createWindowSurface(VkInstance t_instance, VkSurfaceKHR* t_surface);

		// Functions to directly return the window size
		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

		// Function to get the window name
		std::string getWindowName() { return m_windowName; }

	private:
		// Function to initilize the application window
		void initWindow();

		// Window size variables
		const int m_width;
		const int m_height;

		// Window name
		std::string m_windowName;

		// The application window
		GLFWwindow* m_window;

	}; // class AeWindow
} // namespace ae