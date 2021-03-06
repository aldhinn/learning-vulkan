#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/queue_family.h"

namespace vk::tut {
    void Application::create_command_pool() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        QueueFamilyIndices indices = find_family_indices(
            m_physical_device, m_surface
        );
        
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
        result = vkCreateCommandPool(
            m_logical_device, &command_pool_info, nullptr, &m_command_pool
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create command pool."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created command pool.");
    }

    void Application::create_command_buffers() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Create a command buffer per frame buffer.
        m_command_buffers.reserve(m_swapchain_frame_buffers.size());

        for (uint32_t i = 0; i < m_command_buffers.capacity(); i++) {
            VkCommandBufferAllocateInfo command_buffer_info{};
            command_buffer_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_info.commandPool = m_command_pool;
            command_buffer_info.level = VkCommandBufferLevel
                ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_info.commandBufferCount = 1;

            // The command buffer handle to be allocated.
            VkCommandBuffer command_buffer;

            // Allocate command buffer.
            result = vkAllocateCommandBuffers(
                m_logical_device, &command_buffer_info, &command_buffer
            );
            if (result != VkResult::VK_SUCCESS) {
                VK_TUT_LOG_ERROR(
                    "Failed to allocate command buffer."
                );
            }

            m_command_buffers.emplace_back(::std::move(command_buffer));
        }

        VK_TUT_LOG_DEBUG("Successfully allocated command buffers.");
    }

    void Application::destroy_command_pool() {
        // This also frees the command buffers.
        // No need to explicitly free the command buffers.
        vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed command pool.");
    }

    void Application::record_command_buffer(
        const uint32_t& image_index
    ) {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Information about how the command buffer begins recording.
        VkCommandBufferBeginInfo command_buffer_begin_info{};
        command_buffer_begin_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // Optional. Only relevant if we're using secondary command buffers.
        command_buffer_begin_info.pInheritanceInfo = nullptr;
        
        result = vkBeginCommandBuffer(
            m_command_buffers[m_current_frame_index], &command_buffer_begin_info
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to begin recording command buffer."
            );
        }

        // Turn the background into black.
        VkClearValue clear_colour[] = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

        // Information about beginning render pass.
        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = m_render_pass;
        render_pass_begin_info.framebuffer =
            m_swapchain_frame_buffers[image_index];
        render_pass_begin_info.renderArea.offset = {0, 0};
        render_pass_begin_info.renderArea.extent = m_swapchain_extent;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = clear_colour;

        // Begin the render pass.
        vkCmdBeginRenderPass(
            m_command_buffers[m_current_frame_index],
            &render_pass_begin_info,
            VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE
        );
        
        // Bind the command buffer to the graphics pipeline.
        vkCmdBindPipeline(
            m_command_buffers[m_current_frame_index],
            VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_graphics_pipeline
        );

        // Bind the vertex buffers.
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame_index],
            0, 1, &m_mesh_buffer, offsets
        );

        // The data of the index buffer follows after the vertex array.
        VkDeviceSize index_data_offset = static_cast<VkDeviceSize>(
            sizeof(Vertex) * m_vertices.size()
        );
        // Bind the indices data.
        vkCmdBindIndexBuffer(m_command_buffers[m_current_frame_index],
            m_mesh_buffer, index_data_offset,
            VkIndexType::VK_INDEX_TYPE_UINT32
        );

        // Bind the uniform buffers.
        vkCmdBindDescriptorSets(
            m_command_buffers[m_current_frame_index],
            VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_graphics_pipeline_layout, 0, 1,
            &m_descriptor_sets[m_current_frame_index], 0, nullptr
        );

        // Draw the three vertices specified in our vertex shader.
        vkCmdDrawIndexed(
            m_command_buffers[m_current_frame_index],
            static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0
        );

        // End the render pass.
        vkCmdEndRenderPass(m_command_buffers[m_current_frame_index]);

        // End command buffer recording.
        result = vkEndCommandBuffer(m_command_buffers[m_current_frame_index]);
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to record command buffer.");
        }
    }

    VkCommandBuffer begin_single_time_commands(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool
    ) {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Information about the command
        VkCommandBufferAllocateInfo command_buffer_info{};
        command_buffer_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_info.level = VkCommandBufferLevel
            ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_info.commandPool = command_pool;
        command_buffer_info.commandBufferCount = 1;

        // The command buffer that will record the command.
        VkCommandBuffer command_buffer;
        result = vkAllocateCommandBuffers(
            logical_device, &command_buffer_info, &command_buffer
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to allocate command buffer.");
        }
        
        // How the command buffer begins recording.
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VkCommandBufferUsageFlagBits
            ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // Begin recording.
        result = vkBeginCommandBuffer(command_buffer, &begin_info);
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to begin recording command buffer.");
        }

        return command_buffer;
    }

    void end_single_time_commands(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkCommandBuffer& command_buffer
    ) {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // End recording.
        result = vkEndCommandBuffer(command_buffer);
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to record command buffer.");
        }

        // Submit the command.
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_buffer;

        result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to submit the command to the queue.");
        }
        // Wait until the queue is done.
        result = vkQueueWaitIdle(queue);
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to wait for queue.");
        }

        // Free this command buffer as it will
        // no longer be used outside of this scope
        vkFreeCommandBuffers(
            logical_device, command_pool, 1, &command_buffer
        );
    }
}