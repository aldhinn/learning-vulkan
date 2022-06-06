#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/uniform.h"

#include <limits>
#include <chrono>
#include <cstring>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        create_descriptor_set_layout();
        create_graphics_pipeline();
        create_swapchain_frame_buffers();
        create_command_pool();
        load_mesh();
        create_vertex_buffer();
        create_index_buffer();
        create_uniform_buffers();
        create_descriptor_pool();
        create_descriptor_sets();
        create_command_buffers();
        create_sync_objects();

        VK_TUT_LOG_DEBUG("...FINISHED Initializing application data...");
    }

    // Code cleanup.
    Application::~Application() {
        VK_TUT_LOG_DEBUG("...Cleaning up application data...");

        destroy_sync_objects();
        destroy_descriptor_pool();
        destroy_uniform_buffers();
        destroy_index_buffer();
        destroy_vertex_buffer();
        destroy_command_pool();
        destroy_swapchain_frame_buffers();
        destroy_graphics_pipeline();
        destroy_descriptor_set_layout();
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
        vkWaitForFences(m_logical_device, 1, &
            m_in_flight_fences[m_current_frame_index], VK_TRUE,
            ::std::numeric_limits<uint64_t>::max()
        );

        // Acquire the next available image from the swapchain.
        uint32_t image_index = 0;
        VkResult acquire_image_result = vkAcquireNextImageKHR(
            m_logical_device, m_swapchain,
            ::std::numeric_limits<uint64_t>::max(),
            m_image_available_semaphores[m_current_frame_index],
            VK_NULL_HANDLE, &image_index
        );

        if (acquire_image_result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate_swapchain();
            return;
        }
        else if (acquire_image_result != VK_SUCCESS &&
        acquire_image_result != VK_SUBOPTIMAL_KHR) {
            VK_TUT_LOG_ERROR("Failed to acquire swap chain image!");
        }

        // Reset the fence for drawing in the GPU.
        vkResetFences(m_logical_device, 1,
            &m_in_flight_fences[m_current_frame_index]
        );

        // Reset the command buffer.
        vkResetCommandBuffer(m_command_buffers[m_current_frame_index], 0);

        // Record the command buffer with the command that we want.
        // In our case, to draw our triangle.
        record_command_buffer(image_index);

        // Define our wait stages.
        // Our command gets executed in the colour attachment stage in
        // the graphics pipeline finishes.
        VkPipelineStageFlags wait_stages[] = {
            VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        update_uniform_buffer();

        // Information to be submitted to the graphics queue.
        VkSubmitInfo submit_info{};
        submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_command_buffers[m_current_frame_index];
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores =
            &m_image_available_semaphores[m_current_frame_index];
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores =
            &m_render_finished_semaphores[m_current_frame_index];

        // Submit to the graphics queue.
        // Signals the m_in_flight_fence when graphics rendering is done.
        if (vkQueueSubmit(m_graphics_queue, 1, &submit_info,
        m_in_flight_fences[m_current_frame_index]) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to submit draw command buffer.");
        }

        // Presentation information.
        VkPresentInfoKHR present_info{};
        present_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores =
            &m_render_finished_semaphores[m_current_frame_index];
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &m_swapchain;
        present_info.pImageIndices = &image_index;

        // Waits for the graphics rendering before
        // presenting the image back to the swapchain.
        VkResult present_result = vkQueuePresentKHR(
            m_present_queue, &present_info
        );

        if (present_result == VK_ERROR_OUT_OF_DATE_KHR ||
        present_result == VK_SUBOPTIMAL_KHR) {
            recreate_swapchain();
            return;
        }
        else if (present_result != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to present to the swapchain.");
        }

        // Update the current frame index.
        // Will loop back to zero if it exceeded the
        // number of valid framebuffers.
        m_current_frame_index = (m_current_frame_index + 1) %
            m_swapchain_frame_buffers.size();
    }

    void Application::update_uniform_buffer() {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        float time_passed = std::chrono::duration
            <float, std::chrono::seconds::period>
            (current_time - start_time).count();

        Uniform uniform(
            ::glm::rotate(
                ::glm::mat4(1.0f),
                time_passed * ::glm::radians(90.0f),
                ::glm::vec3(0.0f, 0.0f, 1.0f)
            ),
            ::glm::lookAt(
                ::glm::vec3(1.0f, 1.0f, 1.0f),
                ::glm::vec3(0.0f, 0.0f, 0.0f),
                ::glm::vec3(0.0f, 0.0f, 1.0f)
            ),
            ::glm::perspective(
                ::glm::radians(45.0f),
                m_swapchain_extent.width / (float) m_swapchain_extent.height,
                0.1f, 10.0f
            )
        );

        // Update the data of the uniform buffer.
        void* data;
        vkMapMemory(m_logical_device,
            m_uniform_buffer_memories[m_current_frame_index],
            0, sizeof(Uniform), 0, &data
        );
        memcpy(data, &uniform, sizeof(Uniform));
        vkUnmapMemory(m_logical_device,
            m_uniform_buffer_memories[m_current_frame_index]
        );
    }
}