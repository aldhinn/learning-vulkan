#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/QueueFamilyIndices.h"

namespace vk::tut {
    void Application::create_command_pool() {
        QueueFamilyIndices indices =
            find_family_indices(m_physical_device, m_surface);
        
        // If somehow an unsuitable physical device got through.
        if (!indices.is_complete()) {
            VK_TUT_LOG_ERROR(
                "Unsuitable logical device."
            );
        }

        // The information about the command pool.
        VkCommandPoolCreateInfo command_pool_info{};
        command_pool_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_info.queueFamilyIndex = ::std::get<1>(
            indices.get_graphics_family_index()
        );
        command_pool_info.flags = VkCommandPoolCreateFlagBits
            ::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        
        // Create the command pool.
        if (vkCreateCommandPool(m_logical_device, &command_pool_info,
        nullptr, &m_command_pool) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create command pool."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created command pool.");
    }

    void Application::create_command_buffer() {
        VkCommandBufferAllocateInfo command_buffer_info{};
        command_buffer_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_info.commandPool = m_command_pool;
        command_buffer_info.level = VkCommandBufferLevel
            ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_info.commandBufferCount = 1;

        // Allocate command buffer.
        if (vkAllocateCommandBuffers(m_logical_device,
        &command_buffer_info, &m_command_buffer) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to allocate command buffer."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully allocated command buffer.");
    }

    void Application::destroy_command_pool() {
        // This also frees the command buffers.
        // No need to explicitly free the command buffers.
        vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed command pool.");
    }
}