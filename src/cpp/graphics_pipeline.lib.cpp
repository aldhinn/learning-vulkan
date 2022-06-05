#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/shader_parser.h"
#include "vk_tut/vertex.h"

namespace vk::tut {
    void Application::create_graphics_pipeline() {
        VkVertexInputBindingDescription binding_description =
            Vertex::get_binding_description();
        std::array<VkVertexInputAttributeDescription, 2>
        attribute_descriptions = Vertex::get_attribute_descriptions();
        
        VkPipelineVertexInputStateCreateInfo vertex_input_state_info{};
        vertex_input_state_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state_info.vertexBindingDescriptionCount = 1;
        vertex_input_state_info.pVertexBindingDescriptions =
            &binding_description;
        vertex_input_state_info.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_state_info.pVertexAttributeDescriptions =
            attribute_descriptions.data();
        
        // Input assembly stage.
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
        input_assembly_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_info.topology = VkPrimitiveTopology
            ::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_info.primitiveRestartEnable = VK_FALSE;
        
        // Create shader modules.
        m_vertex_shader_module = create_shader_module(m_logical_device,
            _VK_TUT_VERTEX_SHADER_FILEPATH_);
        m_fragment_shader_module = create_shader_module(m_logical_device,
            _VK_TUT_FRAGMENT_SHADER_FILEPATH_);

        // Contains programmable pipeline stages.
        VkPipelineShaderStageCreateInfo shader_stages_info[2]{};

        // Vertex shader stage.
        shader_stages_info[0].sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages_info[0].stage = VkShaderStageFlagBits
            ::VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages_info[0].module = m_vertex_shader_module;
        shader_stages_info[0].pName = "main"; // Entrypoint function name.

        // Fragment shader stage.
        shader_stages_info[1].sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages_info[1].stage = VkShaderStageFlagBits
            ::VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages_info[1].module = m_fragment_shader_module;
        shader_stages_info[1].pName = "main"; // Entrypoint function name.

        // Viewport.
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchain_extent.width);
        viewport.height = static_cast<float>(m_swapchain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor.
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_swapchain_extent;

        // Viewport state.
        VkPipelineViewportStateCreateInfo viewport_state_info{};
        viewport_state_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_info.viewportCount = 1;
        viewport_state_info.pViewports = &viewport;
        viewport_state_info.scissorCount = 1;
        viewport_state_info.pScissors = &scissor;

        // Rasterization.
        VkPipelineRasterizationStateCreateInfo rasterization_info{};
        rasterization_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_info.depthClampEnable = VK_FALSE;
        rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_info.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        rasterization_info.lineWidth = 1.0f;
        rasterization_info.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        rasterization_info.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        rasterization_info.depthBiasEnable = VK_FALSE;

        // Multisampling.
        VkPipelineMultisampleStateCreateInfo multisampling_info{};
        multisampling_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling_info.sampleShadingEnable = VK_FALSE;
        multisampling_info.rasterizationSamples = VkSampleCountFlagBits
            ::VK_SAMPLE_COUNT_1_BIT;
        multisampling_info.minSampleShading = 1.0f; // Optional

        // Colour Blend Attachment.
        VkPipelineColorBlendAttachmentState colour_blend_attachment{};
        colour_blend_attachment.colorWriteMask =
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
        colour_blend_attachment.blendEnable = VK_FALSE;
        colour_blend_attachment.srcColorBlendFactor = VkBlendFactor
            ::VK_BLEND_FACTOR_ONE; // Optional
        colour_blend_attachment.dstColorBlendFactor = VkBlendFactor
            ::VK_BLEND_FACTOR_ZERO; // Optional
        colour_blend_attachment.colorBlendOp = VkBlendOp
            ::VK_BLEND_OP_ADD; // Optional
        colour_blend_attachment.srcAlphaBlendFactor = VkBlendFactor
            ::VK_BLEND_FACTOR_ONE; // Optional
        colour_blend_attachment.dstAlphaBlendFactor = VkBlendFactor
            ::VK_BLEND_FACTOR_ZERO; // Optional
        colour_blend_attachment.alphaBlendOp = VkBlendOp
            ::VK_BLEND_OP_ADD; // Optional
        
        // Colour blending information.
        VkPipelineColorBlendStateCreateInfo colour_blending_info{};
        colour_blending_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colour_blending_info.logicOpEnable = VK_FALSE;
        colour_blending_info.logicOp = VK_LOGIC_OP_COPY; // Optional
        colour_blending_info.attachmentCount = 1;
        colour_blending_info.pAttachments = &colour_blend_attachment;
        colour_blending_info.blendConstants[0] = 0.0f; // Optional
        colour_blending_info.blendConstants[1] = 0.0f; // Optional
        colour_blending_info.blendConstants[2] = 0.0f; // Optional
        colour_blending_info.blendConstants[3] = 0.0f; // Optional

        // Graphics Pipeline layout information.
        VkPipelineLayoutCreateInfo graphics_pipeline_layout_info{};
        graphics_pipeline_layout_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        graphics_pipeline_layout_info.setLayoutCount = 0; // Optional
        graphics_pipeline_layout_info.pSetLayouts = nullptr; // Optional
        graphics_pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        graphics_pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

        // Create the pipeline layout.
        if (vkCreatePipelineLayout(m_logical_device,
        &graphics_pipeline_layout_info, nullptr, &m_graphics_pipeline_layout)
        != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create graphics pipeline layout."
            );
        }

        // Graphics pipeline info.
        VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
        graphics_pipeline_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_info.layout = m_graphics_pipeline_layout;
        graphics_pipeline_info.pVertexInputState = &vertex_input_state_info;
        graphics_pipeline_info.pInputAssemblyState = &input_assembly_info;
        graphics_pipeline_info.stageCount = 2;
        graphics_pipeline_info.pStages = shader_stages_info;
        graphics_pipeline_info.pViewportState = &viewport_state_info;
        graphics_pipeline_info.pRasterizationState = &rasterization_info;
        graphics_pipeline_info.pColorBlendState = &colour_blending_info;
        graphics_pipeline_info.pMultisampleState = &multisampling_info;
        graphics_pipeline_info.renderPass = m_render_pass;

        // Create the graphics pipeline.
        if (vkCreateGraphicsPipelines(m_logical_device, nullptr, 1,
        &graphics_pipeline_info, nullptr, &m_graphics_pipeline)
        != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create graphics pipeline."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created graphics pipeline.");
    }

    void Application::destroy_graphics_pipeline() {
        // Destroy the graphics pipeline itself.
        vkDestroyPipeline(m_logical_device, m_graphics_pipeline, nullptr);
        // Destroy graphics pipeline layout.
        vkDestroyPipelineLayout(m_logical_device, m_graphics_pipeline_layout, nullptr);
        // Destroy shader modules.
        vkDestroyShaderModule(m_logical_device, m_vertex_shader_module, nullptr);
        vkDestroyShaderModule(m_logical_device, m_fragment_shader_module, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed graphics pipeline.");
    }
}