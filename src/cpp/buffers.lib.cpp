#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#include <cstring>

namespace vk::tut {
    void Application::create_vertex_buffer() {
        VkDeviceSize buffer_size = static_cast<VkDeviceSize>(
            sizeof(Vertex) * m_vertices.size()
        );

        // The CPU accessible vertex buffer.
        VkBuffer staging_vertex_buffer;
        // The CPU accessible vertex buffer memory.
        VkDeviceMemory staging_vertex_buffer_memory;

        // Create and allocate the staging buffer.
        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            buffer_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_vertex_buffer,
            &staging_vertex_buffer_memory
        );

        // Filling the buffer.
        void* data;
        vkMapMemory(m_logical_device, staging_vertex_buffer_memory, 0,
            buffer_size, 0, &data
        );
        memcpy(data, m_vertices.data(),
            static_cast<size_t>(buffer_size)
        );
        vkUnmapMemory(m_logical_device, staging_vertex_buffer_memory);

        // Create the actual vertex buffer.
        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            buffer_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_vertex_buffer,
            &m_vertex_buffer_memory
        );

        // Copy the staging buffer to the vertex buffer.
        copy_buffer(
            m_logical_device, m_command_pool, m_graphics_queue,
            staging_vertex_buffer, m_vertex_buffer, buffer_size
        );

        vkFreeMemory(m_logical_device, staging_vertex_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, staging_vertex_buffer, nullptr);

        VK_TUT_LOG_DEBUG("Successfully created and allocated vertex buffer.");
    }

    void Application::create_index_buffer() {
        VkDeviceSize buffer_size = static_cast<VkDeviceSize>(
            sizeof(Vertex) * m_indices.size()
        );

        // The CPU accessible vertex buffer.
        VkBuffer staging_vertex_buffer;
        // The CPU accessible vertex buffer memory.
        VkDeviceMemory staging_vertex_buffer_memory;

        // Create and allocate the staging buffer.
        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            buffer_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_vertex_buffer,
            &staging_vertex_buffer_memory
        );

        // Filling the buffer.
        void* data;
        vkMapMemory(m_logical_device, staging_vertex_buffer_memory, 0,
            buffer_size, 0, &data
        );
        memcpy(data, m_indices.data(),
            static_cast<size_t>(buffer_size)
        );
        vkUnmapMemory(m_logical_device, staging_vertex_buffer_memory);

        // Create the actual index buffer.
        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            buffer_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_index_buffer,
            &m_index_buffer_memory
        );

        // Copy the staging buffer to the vertex buffer.
        copy_buffer(
            m_logical_device, m_command_pool, m_graphics_queue,
            staging_vertex_buffer, m_index_buffer, buffer_size
        );

        vkFreeMemory(m_logical_device, staging_vertex_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, staging_vertex_buffer, nullptr);


        VK_TUT_LOG_DEBUG("Successfully created and allocated index buffer.");
    }

    void Application::destroy_index_buffer() {
        vkFreeMemory(m_logical_device, m_index_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, m_index_buffer, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed index buffer.");
    }

    void Application::destroy_vertex_buffer() {
        vkFreeMemory(m_logical_device, m_vertex_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, m_vertex_buffer, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed vertex buffer.");
    }

    uint32_t find_memory_requirements(
        const VkPhysicalDevice& physical_device,
        // Specify the bit field types that are suitable.
        const uint32_t& type_filter,
        const VkMemoryPropertyFlags& properties
    ) {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

        // Find a memory type that is suitable for the buffer.
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if (type_filter & (1 << i) &&
            (mem_properties.memoryTypes[i].propertyFlags & properties)
            == properties) {
                return i;
            }
        }

        VK_TUT_LOG_ERROR("Failed to find a suitable memory type.");
    }

    void create_and_allocate_buffer(
        const VkPhysicalDevice& physical_device,
        const VkDevice& logical_device,
        const VkDeviceSize& device_size,
        const VkBufferUsageFlags& usage_flags,
        const VkMemoryPropertyFlags& memory_properties,
        VkBuffer* ptr_buffer,
        VkDeviceMemory* ptr_buffer_memory
    ) {
        // Information about the vertex buffer.
        VkBufferCreateInfo vertex_buffer_info{};
        vertex_buffer_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertex_buffer_info.size = device_size;
        vertex_buffer_info.usage = usage_flags;
        vertex_buffer_info.sharingMode = VkSharingMode
            ::VK_SHARING_MODE_EXCLUSIVE;

        // Create the buffer.
        if(vkCreateBuffer(logical_device, &vertex_buffer_info,
        nullptr, ptr_buffer) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create buffer."
            );
        }

        // Get the buffer memory requirements.
        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(logical_device,
            *ptr_buffer, &mem_requirements);

        // Information about the memory to be allocated.
        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_requirements(
            physical_device, mem_requirements.memoryTypeBits,
            memory_properties
        );

        // Allocate memory.
        if (vkAllocateMemory(logical_device, &alloc_info,
        nullptr, ptr_buffer_memory)) {
            VK_TUT_LOG_ERROR("Failed to allocate vertex buffer memory.");
        }

        // Bind the vertex buffer to the memory.
        vkBindBufferMemory(logical_device, *ptr_buffer,
            *ptr_buffer_memory, 0
        );
    }

    void copy_buffer(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkBuffer& src_buffer,
        const VkBuffer& dest_buffer,
        const VkDeviceSize& buffer_size
    ) {
        // Information about the command to copy the src buffer
        // data to the dest buffer data.
        VkCommandBufferAllocateInfo command_buffer_info{};
        command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_info.commandPool = command_pool;
        command_buffer_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(logical_device, &command_buffer_info,
            &command_buffer);
        
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VkCommandBufferUsageFlagBits
            ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // Begin recording copying.
        vkBeginCommandBuffer(command_buffer, &begin_info);

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size = buffer_size;
        vkCmdCopyBuffer(command_buffer, src_buffer,
            dest_buffer, 1, &copy_region);
        
        // End recording copying.
        vkEndCommandBuffer(command_buffer);

        // Submit the copying command.
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_buffer;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        // Wait until the queue is done.
        vkQueueWaitIdle(queue);

        // Free this command buffer as it will
        // no longer be used outside of this scope
        vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
    }
}