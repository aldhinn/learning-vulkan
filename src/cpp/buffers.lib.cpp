#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#include <cstring>

namespace vk::tut {
    void Application::create_vertex_buffer() {
        // Information about the vertex buffer.
        VkBufferCreateInfo vertex_buffer_info{};
        vertex_buffer_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertex_buffer_info.size = sizeof(Vertex) *
            m_vertices.size();
        vertex_buffer_info.usage = VkBufferUsageFlagBits
            ::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        vertex_buffer_info.sharingMode = VkSharingMode
            ::VK_SHARING_MODE_EXCLUSIVE;

        // Create the vertex buffer.
        if(vkCreateBuffer(m_logical_device, &vertex_buffer_info,
        nullptr, &m_vertex_buffer) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create vertex buffer."
            );
        }

        // Get the buffer memory requirements.
        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(m_logical_device,
            m_vertex_buffer, &mem_requirements);

        // Information about the memory to be allocated.
        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_requirements(
            m_physical_device, mem_requirements.memoryTypeBits,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        // Allocate memory.
        if (vkAllocateMemory(m_logical_device, &alloc_info,
        nullptr, &m_vertex_buffer_memory)) {
            VK_TUT_LOG_ERROR("Failed to allocate vertex buffer memory.");
        }

        // Bind the vertex buffer to the memory.
        vkBindBufferMemory(m_logical_device, m_vertex_buffer,
            m_vertex_buffer_memory, 0
        );

        // Filling the buffer.
        void* data;
        vkMapMemory(m_logical_device, m_vertex_buffer_memory, 0,
            vertex_buffer_info.size, 0, &data
        );
        memcpy(data, m_vertices.data(),
            static_cast<size_t>(vertex_buffer_info.size)
        );
        vkUnmapMemory(m_logical_device, m_vertex_buffer_memory);

        VK_TUT_LOG_DEBUG("Successfully created and allocated vertex buffer.");
    }

    void Application::destroy_vertex_buffer() {
        vkDestroyBuffer(m_logical_device, m_vertex_buffer, nullptr);
        vkFreeMemory(m_logical_device, m_vertex_buffer_memory, nullptr);

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
}