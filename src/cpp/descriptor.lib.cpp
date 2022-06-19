#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/uniform.h"

#include <array>

namespace vk::tut {
    void Application::create_descriptor_set_layout() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Descriptor set binding for the uniform object.
        VkDescriptorSetLayoutBinding uniform_layout_binding{};
        uniform_layout_binding.binding = 0;
        uniform_layout_binding.descriptorType = VkDescriptorType
            ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniform_layout_binding.descriptorCount = 1;
        uniform_layout_binding.stageFlags = VkShaderStageFlagBits
            ::VK_SHADER_STAGE_VERTEX_BIT;
        
        // Descriptor set for the texture.
        VkDescriptorSetLayoutBinding texture_layout_binding{};
        texture_layout_binding.binding = 1;
        texture_layout_binding.descriptorCount = 1;
        texture_layout_binding.descriptorType = VkDescriptorType
            ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texture_layout_binding.stageFlags = VkShaderStageFlagBits
            ::VK_SHADER_STAGE_FRAGMENT_BIT;

        ::std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
            uniform_layout_binding, texture_layout_binding
        };

        // Information about the descriptor set layout.
        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
        descriptor_set_layout_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(
            bindings.size()
        );
        descriptor_set_layout_info.pBindings = bindings.data();
        
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

        ::std::array<VkDescriptorPoolSize, 2> descriptor_pool_sizes;

        descriptor_pool_sizes[0].type = VkDescriptorType
            ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_pool_sizes[0].descriptorCount = static_cast<uint32_t>(
            m_swapchain_frame_buffers.size()
        );

        descriptor_pool_sizes[1].type = VkDescriptorType
            ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_pool_sizes[1].descriptorCount = static_cast<uint32_t>(
            m_swapchain_frame_buffers.size()
        );

        VkDescriptorPoolCreateInfo descriptor_pool_info{};
        descriptor_pool_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(
            descriptor_pool_sizes.size()
        );
        descriptor_pool_info.pPoolSizes = descriptor_pool_sizes.data();
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
            VkDescriptorBufferInfo uniform_buffer_info{};
            uniform_buffer_info.offset = 0;
            uniform_buffer_info.buffer = m_uniform_buffers[i];
            uniform_buffer_info.range = static_cast<VkDeviceSize>(
                sizeof(Uniform)
            );

            VkDescriptorImageInfo image_info{};
            image_info.imageLayout = VkImageLayout
                ::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = m_texture_image_view;
            image_info.sampler = m_texture_sampler;

            ::std::array<VkWriteDescriptorSet, 2> descriptor_writes;

            // Descriptor write for the uniform buffer.
            descriptor_writes[0].sType = VkStructureType
                ::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet = m_descriptor_sets[i];
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = VkDescriptorType
                ::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &uniform_buffer_info;
            descriptor_writes[0].pNext = nullptr;

            // Descriptor write for the texture.
            descriptor_writes[1].sType = VkStructureType
                ::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[1].dstSet = m_descriptor_sets[i];
            descriptor_writes[1].dstBinding = 1;
            descriptor_writes[1].dstArrayElement = 0;
            descriptor_writes[1].descriptorType = VkDescriptorType
                ::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_writes[1].descriptorCount = 1;
            descriptor_writes[1].pImageInfo = &image_info;
            descriptor_writes[1].pNext = nullptr;

            vkUpdateDescriptorSets(
                m_logical_device,
                static_cast<uint32_t>(descriptor_writes.size()),
                descriptor_writes.data(), 0, nullptr
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