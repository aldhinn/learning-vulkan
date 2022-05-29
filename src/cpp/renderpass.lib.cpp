#include "vk_tut/application.h"
#include "vk_tut/logging.h"

namespace vk::tut {
    void Application::create_render_pass() {
        // Colour Attachment.
        VkAttachmentDescription colour_attachment{};
        colour_attachment.format = m_swapchain_image_format;
        colour_attachment.samples = VkSampleCountFlagBits
            ::VK_SAMPLE_COUNT_1_BIT;
        colour_attachment.loadOp = VkAttachmentLoadOp
            ::VK_ATTACHMENT_LOAD_OP_CLEAR;
        colour_attachment.storeOp = VkAttachmentStoreOp
            ::VK_ATTACHMENT_STORE_OP_STORE;
        colour_attachment.stencilLoadOp = VkAttachmentLoadOp
            ::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colour_attachment.stencilStoreOp = VkAttachmentStoreOp
            ::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colour_attachment.initialLayout = VkImageLayout
            ::VK_IMAGE_LAYOUT_UNDEFINED;
        colour_attachment.finalLayout = VkImageLayout
            ::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentReference colour_attachment_ref{};
        colour_attachment_ref.attachment = 0;
        colour_attachment_ref.layout = VkImageLayout
            ::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VkPipelineBindPoint
            ::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colour_attachment_ref;

        // Render pass info.
        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &colour_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_logical_device, &render_pass_info,
        nullptr, &m_render_pass) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create render pass."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created render pass.");
    }

    void Application::destroy_render_pass() {
        vkDestroyRenderPass(m_logical_device, m_render_pass, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed render pass.");
    }
}