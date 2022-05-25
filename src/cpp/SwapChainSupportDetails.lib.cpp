#include "vk_tut/SwapChainSupportDetails.h"

#include <utility>

namespace vk::tut {
    // Default Constructor.
    SwapChainSupportDetails::SwapChainSupportDetails(
        VkSurfaceCapabilitiesKHR&& capabilities,
        ::std::vector<VkSurfaceFormatKHR>&& formats,
        ::std::vector<VkPresentModeKHR>&& present_modes
    ) :
    m_capabilities(::std::move(capabilities)),
    m_formats(::std::move(formats)),
    m_present_modes(::std::move(present_modes)) {}

    // Gets the value of m_capabilities.
    VkSurfaceCapabilitiesKHR SwapChainSupportDetails::get_capabilities() {
        return m_capabilities;
    }

    // Gets the value of m_formats.
    ::std::vector<VkSurfaceFormatKHR> SwapChainSupportDetails::get_formats() {
        return m_formats;
    }

    // Gets the value of m_present_modes.
    ::std::vector<VkPresentModeKHR> SwapChainSupportDetails::get_present_modes() {
        return m_present_modes;
    }

    // Describes whether a device has adequate swapchain support.
    bool SwapChainSupportDetails::is_swapchain_support_adequate() {
        return !m_formats.empty() && !m_present_modes.empty();
    }

    // Returns a SwapChainSupportDetails for a certain
    // pair of physical device and surface.
    SwapChainSupportDetails query_swapchain_support(
        const VkPhysicalDevice& physical_device,
        const VkSurfaceKHR& surface
    ) {
        // Retrieve surface capabilities.
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device,
            surface, &capabilities);

        // Retrieve surface formats.
        uint32_t formats_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
            surface, &formats_count, nullptr);
        ::std::vector<VkSurfaceFormatKHR> formats(formats_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
            surface, &formats_count, formats.data());

        // Retrieve present modes.
        uint32_t present_modes_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
            surface, &present_modes_count, nullptr);
        ::std::vector<VkPresentModeKHR> present_modes(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
            surface, &present_modes_count, present_modes.data());

        return SwapChainSupportDetails(
            ::std::move(capabilities),
            ::std::move(formats),
            ::std::move(present_modes)
        );
    }
}