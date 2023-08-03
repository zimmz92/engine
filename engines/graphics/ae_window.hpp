/// \file ae_window.hpp
/// \brief The script defining the window class.
/// The window class is defined.
#pragma once

#define GLFW_INCLUDE_VULKAN

// Libraries
#include <GLFW/glfw3.h>
#include<string>

namespace ae {

    /// A class that creates, and manages, a window using the GLFW library.
	class AeWindow {
	public:
        /// Creates a window using the GLFW library and initialize the size and name.
        /// \param t_w The desired initial width of the GLFW window being created.
        /// \param t_h The desired initial height of the GLFW window being created.
        /// \param t_name The desired initial name of the GLFW window being created.
		AeWindow(int t_w, int t_h, std::string t_name);

		/// Destroys the window created using the GLFW library
		~AeWindow();

		/// Do not allow this class to be copied
		AeWindow(const AeWindow&) = delete;
		AeWindow& operator=(const AeWindow&) = delete;

		/// Do not allow this class to be moved
		AeWindow(AeWindow&&) = delete;
		AeWindow& operator=(AeWindow&&) = delete;

		/// Checks to see if the size of the window has been changed.
		/// \return True if this window has been resized.
		bool wasWindowResized() { return m_frameBufferResized; }

        /// Resets the flag indicating that the size of the window has changed.
		void resetWindowResizedFlag() { m_frameBufferResized = false; }

        /// Gets the GLFW window that is managed by this class.
        /// \return A pointer to the managed GLFW window.
		GLFWwindow* getGLFWwindow() const { return m_window; }

		/// Function to check if the GLFW window wants to close.
		/// \return True if the the managed GLFW window wants to close.
		bool shouldClose() { return glfwWindowShouldClose(m_window); }

		/// Function to get the size of this window in a Vulkan compatible format.
		/// \return A vulkan 2D extent set to the dimensions of this window.
		VkExtent2D getExtent() { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }

		/// Assigns a Vulkan surface to the GLFW window.
		/// \param t_instance The vulkan instance, VkInstance, that is associated with the vulkan surface being assigned
		/// to this window.
		/// \param  t_surface A pointer to the vulkan surface, VkSurfaceKHR, to be assigned to this window.
		void createWindowSurface(VkInstance t_instance, VkSurfaceKHR* t_surface);

        /// Retrieves the width of this window.
        /// \return The width of this window.
		int getWidth() { return m_width; }

        /// Retrieves the height of this window.
        /// \return The height of this window.
		int getHeight() { return m_height; }

		/// Retrieves the name of this window.
		/// \return The name of this window.
		std::string getWindowName() { return m_windowName; }

	private:
		/// Ensures if window is resized the window member variables are updated, and to notify the graphics
		/// frame buffers that they need to update as well.
		/// \param t_window The GLFW window that was resized.
		/// \param t_width The new width of the GLFW window.
		/// \param t_height The new height of the GLFW window.
		static void framebufferResizedCallback(GLFWwindow *t_window, int t_width, int t_height);

		/// Initializes the GLFW window.
		void initWindow();

		/// Width of the GLFW window.
		int m_width;

        /// Height of the GLFW window.
		int m_height;

        /// Flag to indicate that the GLFW window frame buffer has been resized.
		bool m_frameBufferResized = false;

		/// The name of the GLFW window.
		std::string m_windowName;

		/// The GLFW window this managed by this class.
		GLFWwindow* m_window;

	}; // class AeWindow
} // namespace ae