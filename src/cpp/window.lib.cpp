#include "vk_tut/application.h"
#include "vk_tut/logging.h"

namespace vk::tut {
    void Application::create_and_show_window() {
        // Initialize the GLFW library.
        glfwInit();

        // Tell GLFW not to create an OpenGL context.
        // Because DUHHH!! We're doing Vulkan here.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // Create the GLFW Window.
        m_ptr_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
            WINDOW_TITLE, nullptr, nullptr);

        VK_TUT_LOG_DEBUG("Created and showed the window.");
    }

    void Application::create_surface() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        result = glfwCreateWindowSurface(
            m_vulkan_instance, m_ptr_window, nullptr, &m_surface
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create a surface.");
        }

        VK_TUT_LOG_DEBUG("Successfully created a surface.");
    }
    
    void Application::destroy_surface() {
        vkDestroySurfaceKHR(m_vulkan_instance, m_surface, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed surface.");
    }

    void Application::destroy_window() {
        // Destroy the GLFW window that was created.
        glfwDestroyWindow(m_ptr_window);
        // Terminate the GLFW library.
        glfwTerminate();

        VK_TUT_LOG_DEBUG("Destroyed the GLFW Window and terminated GLFW.");
    }

    VkSurfaceFormatKHR choose_surface_format(
        const ::std::vector<VkSurfaceFormatKHR>& formats
    ) {
        for (const VkSurfaceFormatKHR format : formats) {
            if (format.format == VkFormat::VK_FORMAT_B8G8R8_SRGB &&
                format.colorSpace == VkColorSpaceKHR
                ::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            ) return format;
        }

        // Just settle on the first one if the formats aren't perfect.
        return formats[0];
    }
}