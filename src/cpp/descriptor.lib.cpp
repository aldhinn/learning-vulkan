#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/uniform.h"

namespace vk::tut {
    void Application::create_descriptor_set_layout() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Information about descriptor set layout
        // binding for the vertex shader.
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding = 0; // Will bind at index 0.
        layout_binding.descriptorType = VkDescriptorType
            ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layout_binding.descriptorCount = 1;
        layout_binding.stageFlags = VkShaderStageFlagBits
            ::VK_SHADER_STAGE_VERTEX_BIT;

        // Information about the descriptor set layout.
        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
        descriptor_set_layout_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_info.bindingCount = 1;
        descriptor_set_layout_info.pBindings = &layout_binding;
        
        // Create the descriptor set layout.
        result = vkCreateDescriptorSetLayout(
            m_logical_device, &descriptor_set_layout_info,
            nullptr, &m_descriptor_set_layout
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create descriptor set layout."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created descriptor set layout.");
    }

    void Application::create_descriptor_pool() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        VkDescriptorPoolSize descriptor_pool_size{};
        descriptor_pool_size.type = VkDescriptorType
            ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_pool_size.descriptorCount = static_cast<uint32_t>(
            m_swapchain_frame_buffers.size()
        );

        VkDescriptorPoolCreateInfo descriptor_pool_info{};
        descriptor_pool_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_info.poolSizeCount = 1;
        descriptor_pool_info.pPoolSizes = &descriptor_pool_size;
        descriptor_pool_info.maxSets = static_cast<uint32_t>(
            m_swapchain_frame_buffers.size()
        );
        
        result = vkCreateDescriptorPool(
            m_logical_device, &descriptor_pool_info,
            nullptr, &m_descriptor_pool
        );
        if(result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create descriptor pool."
            );
        }
        
        VK_TUT_LOG_DEBUG("Successfully created descriptor pool.");
    }

    void Application::create_descriptor_sets() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Create the same number of descriptor sets as the frame buffers.
        m_descriptor_sets.reserve(m_swapchain_frame_buffers.size());

        for (int i = 0; i < m_descriptor_sets.capacity(); i++) {
            // Information about the descriptor set to be allocated.
            VkDescriptorSetAllocateInfo descriptor_set_alloc_info{};
            descriptor_set_alloc_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptor_set_alloc_info.descriptorPool = m_descriptor_pool;
            descriptor_set_alloc_info.descriptorSetCount = 1;
            descriptor_set_alloc_info.pSetLayouts = &m_descriptor_set_layout;

            // Descriptor set to be allocated.
            VkDescriptorSet descriptor_set;

            result = vkAllocateDescriptorSets(
                m_logical_device, &descriptor_set_alloc_info, &descriptor_set
            );
            if(result != VkResult::VK_SUCCESS) {
                VK_TUT_LOG_ERROR(
                    "Failed to allocate descriptor set."
                );
            }

            m_descriptor_sets.emplace_back(::std::move(descriptor_set));
        }

        // Populate descriptor sets with data.
        for (int i = 0; i < m_descriptor_sets.size(); i++) {
            // Provides handle to the corresponding uniform buffer.
            VkDescriptorBufferInfo descriptor_buffer_info{};
            descriptor_buffer_info.offset = static_cast<VkDeviceSize>(
                i * sizeof(Uniform)
            );
            descriptor_buffer_info.buffer = m_uniform_buffer;
            descriptor_buffer_info.range = static_cast<VkDeviceSize>(
                sizeof(Uniform) * m_descriptor_sets.size()
            );

            VkWriteDescriptorSet descriptor_write{};
            descriptor_write.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write.dstSet = m_descriptor_sets[i];
            descriptor_write.descriptorType = VkDescriptorType
                ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_write.descriptorCount = 1;
            descriptor_write.pBufferInfo = &descriptor_buffer_info;

            vkUpdateDescriptorSets(
                m_logical_device, 1,
                &descriptor_write, 0, nullptr
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created and allocated descriptor sets.");
    }

    void Application::destroy_descriptor_pool() {
        vkDestroyDescriptorPool(m_logical_device,
            m_descriptor_pool, nullptr);
        m_descriptor_sets.clear();

        VK_TUT_LOG_DEBUG("Destroyed descriptor pool.");
    }

    void Application::destroy_descriptor_set_layout() {
        vkDestroyDescriptorSetLayout(m_logical_device,
            m_descriptor_set_layout, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed descriptor set layout.");
    }
}