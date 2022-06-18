#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vk::tut {
    void Application::create_texture_image() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        int texture_width, texture_height, texture_channels;

        stbi_uc* pixels = stbi_load(
            _VK_TUT_TEXTURE_PATH_, &texture_width, &texture_height,
            &texture_channels, STBI_rgb_alpha
        );
        VkDeviceSize image_size = texture_width * texture_height * 4;

        if(!pixels) {
            VK_TUT_LOG_ERROR("Failed to load texture image.");
        }

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        create_and_allocate_buffer(
            m_physical_device,
            m_logical_device,
            image_size,
            VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_buffer, &staging_buffer_memory
        );

        void* data;
        result = vkMapMemory(
            m_logical_device, staging_buffer_memory,
            0, image_size, 0, &data
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to map memory.");
        }
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(m_logical_device, staging_buffer_memory);

        stbi_image_free(pixels);

        create_and_allocate_image(
            m_physical_device, m_logical_device,
            texture_width, texture_height,
            VkFormat::VK_FORMAT_R8G8B8A8_SRGB,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &m_texture_image, &m_texture_image_memory
        );

        transition_image_layout(
            m_logical_device, m_command_pool, m_graphics_queue,
            VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            m_texture_image
        );
        copy_buffer_to_image(
            m_logical_device, m_command_pool, m_graphics_queue,
            static_cast<uint32_t>(texture_width),
            static_cast<uint32_t>(texture_height),
            staging_buffer, m_texture_image
        );
        transition_image_layout(
            m_logical_device, m_command_pool, m_graphics_queue,
            VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            m_texture_image
        );

        vkFreeMemory(m_logical_device, staging_buffer_memory, nullptr);
        vkDestroyBuffer(m_logical_device, staging_buffer, nullptr);

        VK_TUT_LOG_DEBUG("Successfully created texture image.");
    }

    void Application::create_texture_image_view() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        VkImageViewCreateInfo view_info{};
        view_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = m_texture_image;
        view_info.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
        view_info.subresourceRange.aspectMask = VkImageAspectFlagBits
            ::VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(
            m_logical_device, &view_info, nullptr, &m_texture_image_view
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create texture image view.");
        }

        VK_TUT_LOG_DEBUG("Successfully created texture image view.");
    }

    void Application::create_texture_sampler() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);

        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VkFilter::VK_FILTER_LINEAR;
        sampler_info.minFilter = VkFilter::VK_FILTER_LINEAR;
        sampler_info.addressModeU = VkSamplerAddressMode
            ::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VkSamplerAddressMode
            ::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VkSamplerAddressMode
            ::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VkBorderColor
            ::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VkSamplerMipmapMode
            ::VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        result = vkCreateSampler(
            m_logical_device, &sampler_info, nullptr, &m_texture_sampler
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create sampler.");
        }

        VK_TUT_LOG_DEBUG("Successfully created texture sampler.");
    }

    void Application::destroy_texture_sampler() {
        vkDestroySampler(m_logical_device, m_texture_sampler, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed texture sampler.");
    }

    void Application::destroy_texture_image_view() {
        vkDestroyImageView(m_logical_device, m_texture_image_view, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed texture image view.");
    }

    void Application::destroy_texture_image() {
        transition_image_layout(
            m_logical_device, m_command_pool, m_graphics_queue,
            VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            m_texture_image
        );

        vkFreeMemory(m_logical_device, m_texture_image_memory, nullptr);
        vkDestroyImage(m_logical_device, m_texture_image, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed texture image.");
    }

    void create_and_allocate_image(
        const VkPhysicalDevice& physical_device,
        const VkDevice& logical_device,
        const int& width, const int& height,
        const VkFormat& format, const VkImageTiling& tiling,
        const VkImageUsageFlags& usage,
        const VkMemoryPropertyFlags& memory_properties,
        VkImage* ptr_image,
        VkDeviceMemory* ptr_image_memory
    ) {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        VkImageCreateInfo image_info{};
        image_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VkImageType::VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(width);
        image_info.extent.height = static_cast<uint32_t>(height);
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = format;
        image_info.tiling = tiling;
        image_info.initialLayout = VkImageLayout
            ::VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = usage;
        image_info.sharingMode = VkSharingMode
            ::VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VkSampleCountFlagBits
            ::VK_SAMPLE_COUNT_1_BIT;

        result = vkCreateImage(
            logical_device, &image_info,
            nullptr, ptr_image
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create texture image."
            );
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(
            logical_device, *ptr_image, &mem_requirements
        );

        VkMemoryAllocateInfo texture_memory_alloc_info{};
        texture_memory_alloc_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        texture_memory_alloc_info.allocationSize = mem_requirements.size;
        texture_memory_alloc_info.memoryTypeIndex = find_memory_requirements(
            physical_device, mem_requirements.memoryTypeBits,
            memory_properties
        );

        result = vkAllocateMemory(
            logical_device, &texture_memory_alloc_info,
            nullptr, ptr_image_memory
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to allocate texture image memory.");
        }

        vkBindImageMemory(
            logical_device, *ptr_image, *ptr_image_memory, 0
        );
    }

    void copy_buffer_to_image(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const uint32_t& width, const uint32_t& height,
        const VkBuffer& src_buffer, const VkImage& dst_image
    ) {
        VkCommandBuffer copy_command_buffer = begin_single_time_commands(
            logical_device, command_pool
        );

        VkBufferImageCopy copy_region{};
        copy_region.bufferOffset = 0;
        copy_region.bufferRowLength = 0;
        copy_region.bufferImageHeight = 0;

        copy_region.imageSubresource.aspectMask = VkImageAspectFlagBits
            ::VK_IMAGE_ASPECT_COLOR_BIT;
        copy_region.imageSubresource.mipLevel = 0;
        copy_region.imageSubresource.baseArrayLayer = 0;
        copy_region.imageSubresource.layerCount = 1;

        copy_region.imageOffset = {0, 0, 0};
        copy_region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            copy_command_buffer,
            src_buffer,
            dst_image,
            VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &copy_region
        );

        end_single_time_commands(
            logical_device, command_pool, queue, copy_command_buffer
        );
    }

    void transition_image_layout(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkImageLayout& old_layout,
        const VkImageLayout& new_layout,
        const VkImage& image
    ) {
        VkCommandBuffer transition_command_buffer = begin_single_time_commands(
            logical_device, command_pool
        );

        VkImageMemoryBarrier barrier{};
        barrier.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VkImageAspectFlagBits
            ::VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        vkCmdPipelineBarrier(
            transition_command_buffer,
            0 /* TODO */, 0 /* TODO */,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        end_single_time_commands(
            logical_device, command_pool, queue, transition_command_buffer
        );
    }
}