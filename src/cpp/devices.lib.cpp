#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/queue_family.h"
#include "vk_tut/swapchain_support.h"

#include <set>

namespace vk::tut {
    void Application::select_physical_device() {
        // Obtain the handles of the available physical devices.
        uint32_t available_physical_devices_count = 0;
        vkEnumeratePhysicalDevices(m_vulkan_instance,
            &available_physical_devices_count, nullptr);
        ::std::vector<VkPhysicalDevice> available_physical_devices(
            available_physical_devices_count
        );
        vkEnumeratePhysicalDevices(m_vulkan_instance,
            &available_physical_devices_count,
            available_physical_devices.data());
        
        // Select a suitable physical device.
        for (const VkPhysicalDevice& physical_device :
        available_physical_devices) {
            QueueFamilyIndices indices = find_family_indices(
                physical_device, m_surface);

            SwapChainSupportDetails swapchain_support_details =
                query_swapchain_support(physical_device, m_surface);
            
            bool physical_device_suitable = indices.is_complete() &&
                check_device_extension_support(physical_device,
                m_enabled_extensions) &&
                swapchain_support_details.is_swapchain_support_adequate();

            // Select the suitable device.
            if (physical_device_suitable) {
                m_physical_device = physical_device;
                VK_TUT_LOG_DEBUG(
                    "Successfully found a suitable physical device."
                );
                return;
            }
        }
        
        VK_TUT_LOG_ERROR(
            "Failed to find a suitable physical device."
        );
    }

    void Application::create_logical_device() {
        QueueFamilyIndices indices = find_family_indices(
            m_physical_device, m_surface);
        if (!indices.is_complete()) {
            // If an unsuitable device somehow got through.
            VK_TUT_LOG_ERROR("Unsuitable physical device.");
        }

        ::std::vector<uint32_t> unique_family_indices =
            indices.get_unique_queue_family_indices();
        float queue_priority = 1.0f;

        // Information about the device queues to be made.
        ::std::vector<VkDeviceQueueCreateInfo> device_queue_infos;
        // Loop through the unique family indices
        // to populate the device_queue_infos
        for (const uint32_t& index : unique_family_indices) {
            VkDeviceQueueCreateInfo device_queue_info{};
            device_queue_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_info.queueFamilyIndex = index;
            device_queue_info.queueCount = 1;
            device_queue_info.pQueuePriorities = &queue_priority;

            device_queue_infos.emplace_back(device_queue_info);
        }

        // Information about the device features to be enabled.
        VkPhysicalDeviceFeatures enabled_device_features{};

        // Information about the logical device.
        VkDeviceCreateInfo logical_device_info{};
        logical_device_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logical_device_info.queueCreateInfoCount =
            static_cast<uint32_t>(device_queue_infos.size());
        logical_device_info.pQueueCreateInfos =
            device_queue_infos.data();
        logical_device_info.pEnabledFeatures = &enabled_device_features;
        logical_device_info.enabledExtensionCount =
            static_cast<uint32_t>(m_enabled_extensions.size());
        logical_device_info.ppEnabledExtensionNames =
            m_enabled_extensions.data();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        logical_device_info.enabledLayerCount =
            static_cast<uint32_t>(m_enabled_layers_names.size());
        logical_device_info.ppEnabledLayerNames =
            m_enabled_layers_names.data();
#endif
        
        // Create the logical device.
        if(vkCreateDevice(m_physical_device, &logical_device_info,
        nullptr, &m_logical_device) != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create a logical device."
            );
        }

        // Retrieve the present queue handle.
        vkGetDeviceQueue(m_logical_device,
            ::std::get<1>(indices.get_present_family_index()),
            0, &m_present_queue
        );
        // Retrieve the graphics queue handle.
        vkGetDeviceQueue(m_logical_device,
            ::std::get<1>(indices.get_graphics_family_index()),
            0, &m_graphics_queue
        );

        VK_TUT_LOG_DEBUG("Successfully created a logical device.");
    }

    void Application::destroy_logical_device() {
        vkDestroyDevice(m_logical_device, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed logical device.");
    }

    bool check_device_extension_support(
        const VkPhysicalDevice& physical_device,
        const ::std::vector<const char*>& required_extensions
    ) {
        // Retrieve the extensions for the specified physical device.
        uint32_t physical_device_extensions_count = 0;
        vkEnumerateDeviceExtensionProperties(physical_device,
            nullptr, &physical_device_extensions_count, nullptr);
        ::std::vector<VkExtensionProperties> physical_device_extensions(
            physical_device_extensions_count
        );
        vkEnumerateDeviceExtensionProperties(physical_device,
            nullptr, &physical_device_extensions_count,
            physical_device_extensions.data());
        
        // The required_extensions_set will be empty
        // if they all exist in the physical_device_extensions
        ::std::set<::std::string> required_extensions_set(
            required_extensions.begin(), required_extensions.end());
        for (const VkExtensionProperties& extension_prop :
        physical_device_extensions) {
            required_extensions_set.erase(extension_prop.extensionName);
        }

        return required_extensions_set.empty();
    }

    VkPresentModeKHR choose_present_mode(
        const ::std::vector<VkPresentModeKHR>& present_modes
    ) {
        // If VK_PRESENT_MODE_MAILBOX_KHR is available, use that instead.
        for (const VkPresentModeKHR& present_mode : present_modes) {
            if (present_mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
                return present_mode;
        }

        return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    }
}