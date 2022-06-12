#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/swapchain_support.h"
#include "vk_tut/queue_family.h"

#include <stdint.h>
#include <algorithm>

namespace vk::tut {
    void Application::create_swapchain() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        SwapChainSupportDetails swapchan_support = query_swapchain_support(
            m_physical_device, m_surface
        );

        VkSurfaceFormatKHR surface_format = choose_surface_format(
            swapchan_support.get_formats()  
        );
        m_swapchain_image_format = surface_format.format;
        VkPresentModeKHR present_mode = choose_present_mode(
            swapchan_support.get_present_modes()
        );
        m_swapchain_extent = choose_swap_extent(
            swapchan_support.get_capabilities(), m_ptr_window
        );

        uint32_t image_count = swapchan_support
            .get_capabilities().minImageCount + 1;
        // Reference for ::std::clamp
        // https://en.cppreference.com/w/cpp/algorithm/clamp
        // Basically keeping the values
        // of width and height within valid range.
        image_count = ::std::clamp(image_count,
            swapchan_support.get_capabilities().minImageCount,
            swapchan_support.get_capabilities().maxImageCount > 0 ?
                swapchan_support.get_capabilities().maxImageCount :
                ::std::numeric_limits<uint32_t>::max()
        );

        QueueFamilyIndices indices = find_family_indices(
            m_physical_device, m_surface
        );

        // If an invalid physical device somehow got through.
        if (!indices.is_complete()) {
            VK_TUT_LOG_ERROR(
                "Invalid physical device for swapchain creation."
            );
        }

        uint32_t queue_family_indices [] = {
            ::std::get<1>(indices.get_graphics_family_index()),
            ::std::get<1>(indices.get_present_family_index())
        };

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = m_surface;
        swapchain_info.minImageCount = image_count;
        swapchain_info.imageFormat = m_swapchain_image_format;
        swapchain_info.presentMode = present_mode;
        swapchain_info.clipped = VK_TRUE; // Simply clip the obscured pixels.
        swapchain_info.imageExtent = m_swapchain_extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VkImageUsageFlagBits
            ::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.preTransform = swapchan_support
            .get_capabilities().currentTransform;
        // Used for blending with other windows in the
        // windows system. Perhaps, to create some tranclucency
        // effect on the window. We'll set it to opaque for now.
        swapchain_info.compositeAlpha = VkCompositeAlphaFlagBitsKHR
            ::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        if (queue_family_indices[0] != queue_family_indices[1]) {
            swapchain_info.imageSharingMode = VkSharingMode
                ::VK_SHARING_MODE_CONCURRENT;   
            swapchain_info.queueFamilyIndexCount = 2;
            swapchain_info.pQueueFamilyIndices = queue_family_indices;
        }
        else {
            swapchain_info.imageSharingMode = VkSharingMode
                ::VK_SHARING_MODE_EXCLUSIVE;
        }
        
        result = vkCreateSwapchainKHR(
            m_logical_device, &swapchain_info, nullptr, &m_swapchain
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create a swapchain."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created swapchain.");

        // Retrieve swapchain images.
        result = vkGetSwapchainImagesKHR(
            m_logical_device, m_swapchain, &image_count, nullptr
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to get swapchain images.");
        }
        m_swapchain_images.resize(image_count);
        result = vkGetSwapchainImagesKHR(
            m_logical_device, m_swapchain,
            &image_count, m_swapchain_images.data()
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to get swapchain images.");
        }
    }

    void Application::create_swapchain_image_views() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        m_swapchain_image_views.reserve(m_swapchain_images.size());

        // Loop through the swapchain images.
        for (const VkImage& image : m_swapchain_images) {
            VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = image;
            image_view_info.viewType = VkImageViewType
                ::VK_IMAGE_VIEW_TYPE_2D;
            image_view_info.format = m_swapchain_image_format;
            image_view_info.components.r = VkComponentSwizzle
                ::VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.g = VkComponentSwizzle
                ::VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.b = VkComponentSwizzle
                ::VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.a = VkComponentSwizzle
                ::VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.subresourceRange.aspectMask =
                VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_info.subresourceRange.baseMipLevel = 0;
            image_view_info.subresourceRange.levelCount = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount = 1;

            // The image view to be created.
            VkImageView image_view;

            result = vkCreateImageView(
                m_logical_device, &image_view_info, nullptr, &image_view
            );
            if (result != VkResult::VK_SUCCESS) {
                VK_TUT_LOG_ERROR(
                    "Failed to create a swapchain image view."
                );
            }

            m_swapchain_image_views.emplace_back(::std::move(image_view));
        }

        VK_TUT_LOG_DEBUG("Successfully created swapchain image views.");
    }

    void Application::create_swapchain_frame_buffers() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        // Match the size of the image view since each
        // framebuffer matches with an image view.
        m_swapchain_frame_buffers.reserve(m_swapchain_image_views.size());

        // Loop through each image view.
        for (const VkImageView& image_view : m_swapchain_image_views) {
            // The image view the framebuffer is attaching to.
            VkImageView attachments[] = {image_view};

            // The information about the framebuffer to be created.
            VkFramebufferCreateInfo frame_buffer_info{};
            frame_buffer_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frame_buffer_info.renderPass = m_render_pass;
            frame_buffer_info.width = m_swapchain_extent.width;
            frame_buffer_info.height = m_swapchain_extent.height;
            frame_buffer_info.layers = 1;
            frame_buffer_info.attachmentCount = 1;
            frame_buffer_info.pAttachments = attachments;

            // The framebuffer to be created.
            VkFramebuffer frame_buffer;

            // Create the framebuffer.
            result = vkCreateFramebuffer(
                m_logical_device, &frame_buffer_info, nullptr, &frame_buffer
            );
            if (result != VkResult::VK_SUCCESS) {
                VK_TUT_LOG_ERROR(
                    "Failed to create framebuffer."
                );
            }

            m_swapchain_frame_buffers.emplace_back(::std::move(frame_buffer));
        }

        VK_TUT_LOG_DEBUG("Successfully created swapchain framebuffers.");
    }

    void Application::destroy_swapchain_frame_buffers() {
        // Loop through each framebuffer and destroy them.
        for (const VkFramebuffer& frame_buffer : m_swapchain_frame_buffers) {
            vkDestroyFramebuffer(m_logical_device, frame_buffer, nullptr);
        }

        // Clear the list.
        m_swapchain_frame_buffers.clear();
        m_swapchain_frame_buffers.resize(0);
        
        VK_TUT_LOG_DEBUG("Destroyed swapchain framebuffers.");
    }

    void Application::destroy_swapchain_image_views() {
        for (const VkImageView& image_view : m_swapchain_image_views) {
            vkDestroyImageView(m_logical_device, image_view, nullptr);
        }

        m_swapchain_image_views.clear();
        m_swapchain_image_views.resize(0);

        VK_TUT_LOG_DEBUG("Destroyed swapchain image views.");
    }

    void Application::destroy_swapchain() {
        vkDestroySwapchainKHR(m_logical_device, m_swapchain, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed swapchain.");
    }

    void Application::recreate_swapchain() {
        // Wait for resources to be free.
        vkDeviceWaitIdle(m_logical_device);

        // Destroy the current swapchain related objects.
        destroy_swapchain_frame_buffers();
        destroy_graphics_pipeline();
        destroy_render_pass();
        destroy_swapchain_image_views();
        destroy_swapchain();

        // Recreate swapchain and related objects.
        create_swapchain();
        create_swapchain_image_views();
        create_render_pass();
        create_graphics_pipeline();
        create_swapchain_frame_buffers();
    }

    // This dictates the resoultion of the swapchain images.
    VkExtent2D choose_swap_extent(
        const VkSurfaceCapabilitiesKHR& capabilities,
        GLFWwindow* ptr_window
    ) {
        if (capabilities.currentExtent.width !=
        ::std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
        
        int width, height;
        glfwGetFramebufferSize(ptr_window, &width, &height);

        VkExtent2D actual_extent{
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        // Reference for ::std::clamp
        // https://en.cppreference.com/w/cpp/algorithm/clamp
        // Basically keeping the values
        // of width and height within valid range.
        actual_extent.width = ::std::clamp(actual_extent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );
        actual_extent.height = ::std::clamp(actual_extent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );

        return actual_extent;
    }
}