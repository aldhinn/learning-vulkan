#include "vk_tut/application.h"

// These functions will be declared and defined if and
// only if _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

#include "vk_tut/logging.h"
#include <cstring>

namespace vk::tut {
    bool Application::check_validation_layer_support() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        uint32_t available_layers_properties_count;
        result = vkEnumerateInstanceLayerProperties(
            &available_layers_properties_count, nullptr
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to enumerate instance layer properties.");
        }
        std::vector<VkLayerProperties> available_layers_properties(
            available_layers_properties_count
        );
        result = vkEnumerateInstanceLayerProperties(
            &available_layers_properties_count,
            available_layers_properties.data()
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to enumerate instance layer properties.");
        }
        
        for (const VkLayerProperties& layer_prop :
        available_layers_properties) {
            // If VK_LAYER_KHRONOS_validation layer is available,
            // then validation layer is supported.
            if (strcmp("VK_LAYER_KHRONOS_validation",
            layer_prop.layerName) == 0) {
                VK_TUT_LOG_DEBUG("Validation layer is supported");
                return true;
            }
        }

        return false;
    }

    void Application::setup_debug_messenger() {
        // The variable that stores the result of any vulkan function called.
        VkResult result;

        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info{};
        populate_debug_utils_messenger_info(debug_messenger_info);

        result = create_debug_utils_messengerEXT(
            m_vulkan_instance, &debug_messenger_info,
            nullptr, &m_debug_messenger
        );
        if (result != VkResult::VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create debug messenger."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created debug messenger.");
    }

    VkResult create_debug_utils_messengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* ptr_create_info,
        const VkAllocationCallbacks* ptr_allocator,
        VkDebugUtilsMessengerEXT* ptr_debug_messenger
    ) {
        // vkCreateDebugUtilsMessengerEXT is an extension function.
        // Therefore, it needed to be looked up using vkGetInstanceProcAddr
        PFN_vkCreateDebugUtilsMessengerEXT func =
            (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, ptr_create_info, ptr_allocator,
                ptr_debug_messenger);
        }
        else {
            // Function is not present.
            return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroy_debug_utils_messengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debug_messenger,
        const VkAllocationCallbacks* ptr_allocator
    ) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debug_messenger, ptr_allocator);
            VK_TUT_LOG_DEBUG("Destroyed the debug messenger.");
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* ptr_callback_data,
        void* ptr_user_data
    ) {
        // Ignore below error.
        if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            ::std::cerr << "\033[0;92m[" << "VULKAN"
            << "]\033[0m " << ptr_callback_data->pMessage << "\n";
        }

        return VK_FALSE;
    }

    void populate_debug_utils_messenger_info(
        VkDebugUtilsMessengerCreateInfoEXT& ref_debug_messenger_info
    ) {
        ref_debug_messenger_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        ref_debug_messenger_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        ref_debug_messenger_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        ref_debug_messenger_info.pfnUserCallback = debug_callback;
    }
}

#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)