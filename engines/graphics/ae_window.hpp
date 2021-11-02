#pragma once

#define GLFW_INCLUDE_VULKAN

// Libraries
#include <GLFW/glfw3.h>
#include<string>

namespace ae {

	class AeWindow {
	public:
		AeWindow(int w, int h, std::string name);
		~AeWindow();

		// Do not allow this class to be copied (2 lines below)
		AeWindow(const AeWindow&) = delete;
		AeWindow& operator=(const AeWindow&) = delete;

		// Do not allow this class to be moved (2 lines below)
		AeWindow(AeWindow&&) = delete;
		AeWindow& operator=(AeWindow&&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		int getWidth() { return width; }
		int getHeight() { return height; }

		std::string getWindowName() { return windowName; }

	private:
		void initWindow();

		const int width;
		const int height;

		std::string windowName;
		GLFWwindow* window;
	}; // class AeWindow

} // namespace ae