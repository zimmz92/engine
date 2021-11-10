#pragma once

#define GLFW_INCLUDE_VULKAN

// Libraries
#include <GLFW/glfw3.h>
#include<string>

namespace ae {

	class AeWindow {
	public:
		AeWindow(int t_w, int t_h, std::string t_name);
		~AeWindow();

		// Do not allow this class to be copied (2 lines below)
		AeWindow(const AeWindow&) = delete;
		AeWindow& operator=(const AeWindow&) = delete;

		// Do not allow this class to be moved (2 lines below)
		AeWindow(AeWindow&&) = delete;
		AeWindow& operator=(AeWindow&&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(m_window); }
		VkExtent2D getExtent() { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }

		void createWindowSurface(VkInstance t_instance, VkSurfaceKHR* t_surface);

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

		std::string getWindowName() { return m_windowName; }

	private:
		void initWindow();

		const int m_width;
		const int m_height;

		std::string m_windowName;
		GLFWwindow* m_window;
	}; // class AeWindow

} // namespace ae