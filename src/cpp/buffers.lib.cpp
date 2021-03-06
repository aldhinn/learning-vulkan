#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/uniform.h"

#include <cstring>

namespace vk::tut {
    void Application::create_mesh_buffer() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        VkDeviceSize objects_buffer_size = static_cast<VkDeviceSize>(
            sizeof(Vertex) * m_vertices.size() +
            sizeof(uint32_t) * m_indices.size()
        );

        // The CPU accessible objects buffer.
        VkBuffer staging_objects_buffer;
        // The CPU accessible objects buffer memory.
        VkDeviceMemory staging_objects_buffer_memory;

        // Create and allocate the staging buffer.
        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            objects_buffer_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_objects_buffer,
            &staging_objects_buffer_memory
        );

        // Filling the buffer.
        void* data;
        result = vkMapMemory(
            m_logical_device, staging_objects_buffer_memory, 0,
            objects_buffer_size, 0, &data
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to map memory.");
        }
        // Fill in with the vertices data.
        memcpy(
            data,
            m_vertices.data(),
            static_cast<size_t>(sizeof(Vertex) * m_vertices.size())
        );
        // Append the data buffer with the indices data.
        memcpy(
            static_cast<void*>(
                static_cast<char*>(data) +
                static_cast<uint32_t>(sizeof(Vertex) * m_vertices.size())
            ),
            m_indices.data(),
            static_cast<size_t>(sizeof(uint32_t) * m_indices.size())
        );
        vkUnmapMemory(m_logical_device, staging_objects_buffer_memory);

        // Create the actual vertex buffer.
        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            objects_buffer_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_mesh_buffer,
            &m_mesh_buffer_memory
        );

        // Copy the staging buffer to the vertex buffer.
        copy_buffer(
            m_logical_device, m_command_pool, m_graphics_queue,
            staging_objects_buffer, m_mesh_buffer, objects_buffer_size
        );

        vkFreeMemory(m_logical_device, staging_objects_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, staging_objects_buffer, nullptr);

        VK_TUT_LOG_DEBUG("Successfully created and allocated objects buffer.");
    }

    void Application::create_uniform_buffers() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Allocate a uniform buffer with the size of Uniform times
        // the number of swapchain frame buffers.
        m_uniform_buffers.reserve(m_swapchain_frame_buffers.size());
        m_uniform_buffer_memories.reserve(m_uniform_buffers.size());

        for (int i = 0; i < m_uniform_buffers.capacity(); i++) {
            // The uniform buffer to be created.
            VkBuffer uniform_buffer;
            // The uniform buffer device memory to be allocated to.
            VkDeviceMemory uniform_buffer_memory;

            create_and_allocate_buffer(
                m_physical_device, m_logical_device,
                static_cast<VkDeviceSize>(
                    sizeof(Uniform)
                ),
                VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                &uniform_buffer, &uniform_buffer_memory
            );

            m_uniform_buffers.emplace_back(::std::move(uniform_buffer));
            m_uniform_buffer_memories.emplace_back(
                ::std::move(uniform_buffer_memory)
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created and allocated uniform buffers.");
    }

    void Application::destroy_uniform_buffers() {
        for (const VkDeviceMemory& uniform_buffer_memory :
        m_uniform_buffer_memories) {
            vkFreeMemory(m_logical_device, uniform_buffer_memory, nullptr);
        }
        m_uniform_buffer_memories.clear();
        for (const VkBuffer& uniform_buffer : m_uniform_buffers) {
            vkDestroyBuffer(m_logical_device, uniform_buffer, nullptr);
        }
        m_uniform_buffers.clear();

        VK_TUT_LOG_DEBUG("Destroyed uniform buffer.");
    }

    void Application::destroy_mesh_buffer() {
        vkFreeMemory(m_logical_device, m_mesh_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, m_mesh_buffer, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed mesh buffer.");
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
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Information about the buffer to be created.
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = device_size;
        buffer_info.usage = usage_flags;
        buffer_info.sharingMode = VkSharingMode
            ::VK_SHARING_MODE_EXCLUSIVE;

        // Create the buffer.
        result = vkCreateBuffer(
            logical_device, &buffer_info, nullptr, ptr_buffer
        );
        if(result != VkResult::VK_SUCCESS) {
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
        result = vkAllocateMemory(
            logical_device, &alloc_info, nullptr, ptr_buffer_memory
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to allocate buffer memory.");
        }

        // Bind the vertex buffer to the memory.
        result = vkBindBufferMemory(
            logical_device, *ptr_buffer, *ptr_buffer_memory, 0
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to bind buffer memory.");
        }
    }

    void copy_buffer(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkBuffer& src_buffer,
        const VkBuffer& dest_buffer,
        const VkDeviceSize& buffer_size
    ) {
        VkCommandBuffer copy_command_buffer = begin_single_time_commands(
            logical_device, command_pool
        );
        
        // Information about how the copy happens.
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size = buffer_size;
        vkCmdCopyBuffer(
            copy_command_buffer, src_buffer,
            dest_buffer, 1, &copy_region
        );

        end_single_time_commands(
            logical_device, command_pool, queue, copy_command_buffer
        );
    }
}