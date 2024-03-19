/// \file ae_window.cpp
/// \brief The script implementing the window class.
/// The window class is implemented.
#include "ae_window.hpp"

// libraries
#include <stdexcept>

namespace ae {
	// AeWindow class constructor
	AeWindow::AeWindow(int t_w, int t_h, std::string t_name) : m_width{t_w}, m_height{t_h}, m_windowName{t_name} {
        // Initialize the GLFW window managed by this class.
		initWindow();
	}

	// AeWindow class destructor
	AeWindow::~AeWindow() {
        // Destroy the GLFW window that was managed by this class.
		glfwDestroyWindow(m_window);

        // Terminate the GLFW library
		glfwTerminate();
	}

	// Function to initialize the application window
	void AeWindow::initWindow() {
        // Initialize the GLFW library.
		glfwInit();

		// Turn off the GLFW OpenGL default integration since we are using Vulkan not OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Do not allow the window to resize, this will be handled by another portion of the vulkan engine
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// Create the window using GLFW
		m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);

		// Associate this class with the created GLFW window to facilitate updating this class if window features
        // change.
		glfwSetWindowUserPointer(m_window, this);

        // Ensure that if the frame buffer (window size) of the created GLFW window is changed the function in this
        // class that handles that scenario is called.
		glfwSetFramebufferSizeCallback(m_window, framebufferResizedCallback);
	}

	// Add the Vulkan surface to the GLFW window
	void AeWindow::createWindowSurface(VkInstance t_instance, VkSurfaceKHR* t_surface) {
		// Attempt to create the GLFW window surface using the vulkan surface, if it fails throw an error.
		if (glfwCreateWindowSurface(t_instance, m_window, nullptr, t_surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	// Ensure if window frame buffer (size) updates class attributes and flags are updated accordingly.
	void AeWindow::framebufferResizedCallback(GLFWwindow *t_window, int t_width, int t_height) {
        // Get the instantiation of this class that is associated with the GLFW window that had its frame buffer
        // resized.
		auto aeWindow = reinterpret_cast<AeWindow *>(glfwGetWindowUserPointer(t_window));

        // Update the window width and height tracking variables
		aeWindow->m_width = t_width;
		aeWindow->m_height = t_height;

        // Set the flag to alert external users of this class that the window frame buffer (size) has been changed.
        aeWindow->m_frameBufferResized = true;
	}

} //namespace ae