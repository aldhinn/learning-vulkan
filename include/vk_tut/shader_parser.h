#if !defined(_VK_TUT_SHADERS_HEADER_)
#define _VK_TUT_SHADERS_HEADER_

// This header files contain functions regarding shader parsing.

// C++ only region.
#if defined(__cplusplus)

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

namespace vk::tut {
    using byte_t = char; // 8-bit data type.
    using file_data_t = ::std::vector<byte_t>;

    file_data_t get_file_data(const ::std::string& filepath);
    VkShaderModule create_shader_module(
        const VkDevice& logical_device,
        const ::std::string& shader_filepath
    );
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.