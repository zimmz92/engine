#include "ae_window.hpp"

// libraries
#include <stdexcept>

namespace ae {
	// AeWindow class constructor
	AeWindow::AeWindow(int t_w, int t_h, std::string t_name) : m_width{t_w}, m_height{t_h}, m_windowName{t_name} {
		initWindow();
	}

	// AeWindow class destructor
	AeWindow::~AeWindow() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	// Function to initilize the application window
	void AeWindow::initWindow() {
		glfwInit();

		// Turn of GLFW Open GL default integration
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Do not allow the window to resize
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// Create the application window using GLFW
		m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
	}

	// Function to add the Vulkan surface to the GLFW window
	void AeWindow::createWindowSurface(VkInstance t_instance, VkSurfaceKHR* t_surface) {
		// Attempt to create the glfw window, if it fails throw an error
		if (glfwCreateWindowSurface(t_instance, m_window, nullptr, t_surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}
} //namespace ae