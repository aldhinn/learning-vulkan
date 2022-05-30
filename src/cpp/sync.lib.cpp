#include "vk_tut/application.h"
#include "vk_tut/logging.h"

namespace vk::tut {
    void Application::create_sync_objects() {
        // Create image available semaphore for each frame buffer.
        m_image_available_semaphores.reserve(m_swapchain_frame_buffers.size());
        for (uint32_t i = 0; i < m_image_available_semaphores.capacity();
        i++) {
            // Information about image available semaphore.
            VkSemaphoreCreateInfo image_available_semaphore_info{};
            image_available_semaphore_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            // The semaphore object to be created.
            VkSemaphore image_available_semaphore;

            if (vkCreateSemaphore(m_logical_device,
            &image_available_semaphore_info, nullptr,
            &image_available_semaphore) != VK_SUCCESS) {
                VK_TUT_LOG_ERROR(
                    "Failed to create image_available_semaphore."
                );
            }

            m_image_available_semaphores.emplace_back(::std::move(
                image_available_semaphore
            ));
        }

        // Create render finish semaphore for each frame buffer.
        m_render_finished_semaphores.reserve(m_swapchain_frame_buffers.size());
        for (uint32_t i = 0; i < m_render_finished_semaphores.capacity();
        i++) {
            // Information about render finished semaphore.
            VkSemaphoreCreateInfo render_finished_semaphore_info{};
            render_finished_semaphore_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            // The semaphore object to be created.
            VkSemaphore render_finished_semaphore;

            if (vkCreateSemaphore(m_logical_device,
            &render_finished_semaphore_info, nullptr,
            &render_finished_semaphore) != VK_SUCCESS) {
                VK_TUT_LOG_ERROR("Failed to create render_finished_semaphore.");
            }

            m_render_finished_semaphores.emplace_back(::std::move(
                render_finished_semaphore
            ));
        }

        // Create in flight fences for each frame buffer.
        m_in_flight_fences.reserve(m_swapchain_frame_buffers.size());
        for (uint32_t i = 0; i < m_in_flight_fences.capacity();
        i++) {
            VkFenceCreateInfo in_flight_fence_info{};
            in_flight_fence_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            in_flight_fence_info.flags = VkFenceCreateFlagBits
                ::VK_FENCE_CREATE_SIGNALED_BIT;
            
            // The fence to be created.
            VkFence in_flight_fence;

            if (vkCreateFence(m_logical_device, &in_flight_fence_info,
            nullptr, &in_flight_fence) != VK_SUCCESS) {
                VK_TUT_LOG_ERROR("Failed to create in_flight_fence.");
            }

            m_in_flight_fences.emplace_back(::std::move(in_flight_fence));
        }

        VK_TUT_LOG_DEBUG("Successfully created sync objects.");
    }

    void Application::destroy_sync_objects() {
        for (const VkSemaphore& image_available_semaphore :
        m_image_available_semaphores) {
            vkDestroySemaphore(m_logical_device,
                image_available_semaphore, nullptr
            );
        }
        m_image_available_semaphores.clear();
        m_image_available_semaphores.resize(0);

        for (const VkSemaphore& render_finished_semaphore :
        m_render_finished_semaphores) {
            vkDestroySemaphore(m_logical_device,
                render_finished_semaphore, nullptr
            );
        }
        m_render_finished_semaphores.clear();
        m_render_finished_semaphores.resize(0);

        for (const VkFence& in_flight_fence : m_in_flight_fences) {
            vkDestroyFence(m_logical_device,
                in_flight_fence, nullptr
            );
        }
        m_in_flight_fences.clear();
        m_in_flight_fences.resize(0);

        VK_TUT_LOG_DEBUG("Destroyed sync objects.");
    }
}