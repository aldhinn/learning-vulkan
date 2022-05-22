#include "vk_tut/QueueFamilyIndices.h"
#include "vk_tut/logging.h"

#include <vector>

namespace vk::tut {
    // If all the indices have value.
    bool QueueFamilyIndices::is_complete() {
        return m_graphics_family_index.has_value() &&
        m_present_family_index.has_value();
    }

    // Sets the value of m_graphics_family_index.
    void QueueFamilyIndices::set_graphics_family_index(uint32_t index) {
        m_graphics_family_index = index;
    }

    // Get the value of the m_graphics_family_index
    // First value is true if m_graphics_family_index has value.
    // Second value is m_graphics_family_index.value()
    ::std::tuple<bool, uint32_t> QueueFamilyIndices
    ::get_graphics_family_index() {
        if (!m_graphics_family_index.has_value()) {
            return ::std::make_tuple(false, 0);
        }

        return ::std::make_tuple(true, m_graphics_family_index.value());
    }

    // Sets the value of m_present_family_index.
    void QueueFamilyIndices::set_present_family_index(uint32_t index) {
        m_present_family_index = index;
    }

    // Get the value of the m_present_family_index.
    // First value is true if m_present_family_index has value.
    // Second value is m_present_family_index.value()
    ::std::tuple<bool, uint32_t> QueueFamilyIndices::get_present_family_index() {
        if (!m_present_family_index.has_value()) {
            return ::std::make_tuple(false, 0);
        }

        return ::std::make_tuple(true, m_present_family_index.value());
    }

    // Find the family indices of a specific physical device.
    QueueFamilyIndices find_family_indices(
        const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface
    ) {
        QueueFamilyIndices result;

        // Obtain the queue family properties of the physical device.
        uint32_t queue_family_props_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
            &queue_family_props_count, nullptr);
        ::std::vector<VkQueueFamilyProperties> queue_family_props(
            queue_family_props_count
        );
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
            &queue_family_props_count, queue_family_props.data());
        
        // The index of the current queue_family_prop being evaluated.
        uint32_t current_index = 0;
        // Loop through the queue_family_props
        for (const VkQueueFamilyProperties& queue_family_prop :
        queue_family_props) {
            // Querying for graphics capability.
            if (queue_family_prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                result.set_graphics_family_index(current_index);
            }

            // Querying for present support.
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device,
                current_index, surface, &present_support);
            if (present_support) {
                result.set_present_family_index(current_index);
            }

            // If the desired indices are populated, then there would
            // be no reason to loop through for more.
            if (result.is_complete()) {
                VK_TUT_LOG_DEBUG("Complete QueueFamilyIndices.");
                return result;
            }

            current_index++;
        }

        VK_TUT_LOG_DEBUG("Incomplete QueueFamilyIndices.");
        return result;
    }
}