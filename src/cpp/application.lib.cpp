#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#include <limits>

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
        create_sync_objects();

        VK_TUT_LOG_DEBUG("...FINISHED Initializing application data...");
    }

    // Code cleanup.
    Application::~Application() {
        VK_TUT_LOG_DEBUG("...Cleaning up application data...");

        destroy_sync_objects();
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
            draw_frame();
        }

        vkDeviceWaitIdle(m_logical_device);
    }

    void Application::draw_frame() {
        // Wait until the previous frame has finished rendering in the GPU.
        vkWaitForFences(m_logical_device, 1, &m_in_flight_fence,
            VK_TRUE, ::std::numeric_limits<uint64_t>::max());
        // Reset the fence for drawing in the GPU.
        vkResetFences(m_logical_device, 1, &m_in_flight_fence);

        // Acquire the next available image from the swapchain.
        uint32_t image_index = 0;
        vkAcquireNextImageKHR(m_logical_device, m_swapchain,
            ::std::numeric_limits<uint64_t>::max(),
            m_image_available_semaphore, VK_NULL_HANDLE, &image_index
        );

        // Reset the command buffer.
        vkResetCommandBuffer(m_command_buffer, 0);

        // Record the command buffer with the command that we want.
        // In our case, to draw our triangle.
        record_command_buffer(image_index);

        // Define our wait stages.
        // Our command gets executed in the colour attachment stage in
        // the graphics pipeline finishes.
        VkPipelineStageFlags wait_stages[] = {
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        // Information to be submitted to the graphics queue.
        VkSubmitInfo submit_info{};
        submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_command_buffer;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &m_image_available_semaphore;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &m_render_finished_semaphore;

        // Submit to the graphics queue.
        // Signals the m_in_flight_fence when graphics rendering is done.
        if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_in_flight_fence)
        != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to submit draw command buffer.");
        }

        // Presentation information.
        VkPresentInfoKHR present_info{};
        present_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &m_render_finished_semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &m_swapchain;
        present_info.pImageIndices = &image_index;

        // Waits for the graphics rendering before
        // presenting the image back to the swapchain.
        if (vkQueuePresentKHR(m_present_queue, &present_info)
        != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to present to the swapchain.");
        }
    }
}