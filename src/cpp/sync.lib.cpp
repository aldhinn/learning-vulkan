#include "vk_tut/application.h"
#include "vk_tut/logging.h"

namespace vk::tut {
    void Application::create_sync_objects() {
        // Information about image available semaphore.
        VkSemaphoreCreateInfo image_available_semaphore_info{};
        image_available_semaphore_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        if (vkCreateSemaphore(m_logical_device, &image_available_semaphore_info,
        nullptr, &m_image_available_semaphore) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create m_image_available_semaphore.");
        }

        // Information about render finished semaphore.
        VkSemaphoreCreateInfo render_finished_semaphore_info{};
        render_finished_semaphore_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        if (vkCreateSemaphore(m_logical_device, &render_finished_semaphore_info,
        nullptr, &m_render_finished_semaphore) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create m_render_finished_semaphore.");
        }

        VkFenceCreateInfo in_flight_fence_info{};
        in_flight_fence_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        in_flight_fence_info.flags = VkFenceCreateFlagBits
            ::VK_FENCE_CREATE_SIGNALED_BIT;
        
        if (vkCreateFence(m_logical_device, &in_flight_fence_info,
        nullptr, &m_in_flight_fence) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create m_in_flight_fence.");
        }

        VK_TUT_LOG_DEBUG("Successfully created sync objects.");
    }

    void Application::destroy_sync_objects() {
        vkDestroySemaphore(m_logical_device, m_image_available_semaphore,
            nullptr);
        vkDestroySemaphore(m_logical_device, m_render_finished_semaphore,
            nullptr);
        vkDestroyFence(m_logical_device, m_in_flight_fence, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed sync objects.");
    }
}