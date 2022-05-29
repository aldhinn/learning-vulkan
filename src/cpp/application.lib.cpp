#include "vk_tut/application.h"
#include "vk_tut/logging.h"

namespace vk::tut {
    // Default constructor.
    Application::Application() {
        VK_TUT_LOG_DEBUG("...Initializing application data...");

        create_and_show_window();
        init_vulkan_instance();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        setup_debug_messenger();
#endif
        create_surface();
        select_physical_device();
        create_logical_device();
        create_swapchain();
        create_swapchain_image_views();
        create_render_pass();
        create_graphics_pipeline();
        create_swapchain_frame_buffers();
        create_command_pool();
        create_command_buffer();

        VK_TUT_LOG_DEBUG("...FINISHED Initializing application data...");
    }

    // Code cleanup.
    Application::~Application() {
        VK_TUT_LOG_DEBUG("...Cleaning up application data...");

        destroy_command_pool();
        destroy_swapchain_frame_buffers();
        destroy_graphics_pipeline();
        destroy_render_pass();
        destroy_swapchain_image_views();
        destroy_swapchain();
        destroy_logical_device();
        destroy_surface();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        destroy_debug_utils_messengerEXT(m_vulkan_instance,
            m_debug_messenger, nullptr);
#endif
        destroy_vulkan_instance();
        destroy_window();

        VK_TUT_LOG_DEBUG("...FINISHED cleaning up application data...");
    }

    // Runs the application loop.
    void Application::run() {
        VK_TUT_LOG_DEBUG("Running the application.");

        // The main application loop.
        // This keeps running until a close event is received by
        // the GLFW API.
        while(!glfwWindowShouldClose(m_ptr_window)) {
            glfwPollEvents();
        }
    }
}