#if !defined(_VK_TUT_QUEUE_FAMILY_INDICES_HEADER_)
#define _VK_TUT_QUEUE_FAMILY_INDICES_HEADER_

// C++ only region.
#if defined(__cplusplus)

#include <vulkan/vulkan.h>
#include <optional>
#include <cstdlib>
#include <tuple>

namespace vk::tut {
    // The indices of a certain physical device queue
    // family with certain interesting queue flags.
    class QueueFamilyIndices final {
    public:
        // Default constructor.
        inline QueueFamilyIndices() {}

        // If all the desired indices have value.
        bool is_complete();

        // Sets the value of m_graphics_family_index.
        void set_graphics_family_index(uint32_t index);
        // Get the value of the m_graphics_family_index.
        // First value is true if m_graphics_family_index has value.
        // Second value is m_graphics_family_index.value()
        ::std::tuple<bool, uint32_t> get_graphics_family_index();

    private:
        // The index of a queue family that has the
        // VK_QUEUE_GRAPHICS_BIT raised in the queue flags.
        ::std::optional<uint32_t> m_graphics_family_index;
    };

    // Find the family indices of a specific physical device.
    QueueFamilyIndices find_family_indices(
        const VkPhysicalDevice& physical_device
    );
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.