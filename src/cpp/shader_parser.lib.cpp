#include "vk_tut/shader_parser.h"
#include "vk_tut/logging.h"

#include <filesystem>
#include <fstream>
#include <cstdlib>

namespace vk::tut {
    file_data_t get_file_data(const ::std::string& filepath) {
        // Create a file handler opening in binary format,
        // and pointing to the end of the file.
        ::std::ifstream shader_file_handle(
            filepath, ::std::ios::ate | ::std::ios::binary
        );

        if (!shader_file_handle.is_open()) {
            VK_TUT_LOG_ERROR(
                "Unable to open specified path : " + filepath
            );
        }

        size_t file_size = static_cast<size_t>(shader_file_handle.tellg());
        // Create the data buffer allocating the size of the file.
        file_data_t buffer(file_size);
        // Point back to the begining of the file.
        shader_file_handle.seekg(0);
        // Read the file contents into buffer.
        shader_file_handle.read(buffer.data(),
            static_cast<::std::streamsize>(file_size));
        // Close the file handle.
        shader_file_handle.close();

        return buffer;
    }

    VkShaderModule create_shader_module(
        const VkDevice& logical_device,
        const ::std::string& shader_filepath
    ) {
        // Get the shader data.
        file_data_t shader_data = get_file_data(shader_filepath);

        VkShaderModuleCreateInfo shader_module_info{};
        shader_module_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_info.codeSize = static_cast<size_t>(
            shader_data.size()
        );
        shader_module_info.pCode = reinterpret_cast<uint32_t*>(
            shader_data.data()
        );

        // The shader module to be created.
        VkShaderModule shader_module;

        if (vkCreateShaderModule(logical_device,
        &shader_module_info, nullptr, &shader_module) 
        != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create shader module."
            );
        }

        return shader_module;
    }
}