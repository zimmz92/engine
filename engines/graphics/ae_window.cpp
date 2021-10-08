#include "ae_window.hpp"

// libraries
#include <stdexcept>

namespace ae {
	// AeWindow class constructor
	AeWindow::AeWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
		initWindow();
	}

	// AeWindow class destructor
	AeWindow::~AeWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void AeWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

	void AeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}
} //namespace ae