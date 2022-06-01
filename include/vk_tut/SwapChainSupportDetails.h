#if !defined(_VK_TUT_SWAPCHAIN_SUPPORT_DETAILS_HEADER_)
#define _VK_TUT_SWAPCHAIN_SUPPORT_DETAILS_HEADER_

// C++ only region.
#if defined(__cplusplus)

#include <vulkan/vulkan.h>
#include <vector>

namespace vk::tut {
    // Contains details about a physical device's swapchain support.
    class SwapChainSupportDetails final {
    public:
        // Delete no init constructor.
        inline SwapChainSupportDetails() = delete;
        // Move initializer list constructor.
        SwapChainSupportDetails(
            VkSurfaceCapabilitiesKHR&& capabilities,
            ::std::vector<VkSurfaceFormatKHR>&& formats,
            ::std::vector<VkPresentModeKHR>&& present_modes
        );

        // Prevent copying.
        inline SwapChainSupportDetails(const SwapChainSupportDetails&) = delete;
        // Move constructor.
        SwapChainSupportDetails(SwapChainSupportDetails&&);

        // Prevent copy re-assignment.
        inline constexpr SwapChainSupportDetails&
        operator= (const SwapChainSupportDetails&) = delete;
        // Move re-assignment.
        SwapChainSupportDetails& operator= (SwapChainSupportDetails&&);

        // Gets the value of m_capabilities.
        VkSurfaceCapabilitiesKHR get_capabilities() const;
        // Gets the value of m_formats.
        ::std::vector<VkSurfaceFormatKHR> get_formats() const;
        // Gets the value of m_present_modes.
        ::std::vector<VkPresentModeKHR> get_present_modes() const;

        // Describes whether a device has adequate swapchain support.
        bool is_swapchain_support_adequate() const;

    private:
        VkSurfaceCapabilitiesKHR m_capabilities;
        ::std::vector<VkSurfaceFormatKHR> m_formats;
        ::std::vector<VkPresentModeKHR> m_present_modes;
    };

    // Returns a SwapChainSupportDetails for a certain
    // pair of physical device and surface.
    SwapChainSupportDetails query_swapchain_support(
        const VkPhysicalDevice& physical_device,
        const VkSurfaceKHR& surface
    );
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.