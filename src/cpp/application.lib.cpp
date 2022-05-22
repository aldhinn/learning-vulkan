#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#include <cstring>
#include <tuple>

namespace vk::tut {
    // Default constructor.
    Application::Application() {
        create_and_show_window();
        init_vulkan_instance();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        setup_debug_messanger();
#endif
        select_physical_devices();
        create_logical_devices();
    }

    // Code cleanup.
    Application::~Application() {
        VK_TUT_LOG_DEBUG("Cleaning up application data...");

        destroy_logical_devices();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        destroy_debug_utils_messengerEXT(m_vulkan_instance,
            m_debug_messanger, nullptr);
#endif
        destroy_vulkan_instance();
        destroy_window();

        VK_TUT_LOG_DEBUG("FINISHED cleaning up application data...");
    }

    // Runs the application loop.
    void Application::run() {
        VK_TUT_LOG_DEBUG("Running the application.");

        // The main application loop.
        // This keeps running until a close event is received by
        // the GLFW API.
        while(!glfwWindowShouldClose(m_ptr_window)) {
            glfwPollEvents();
        }
    }

    // < -------------------- Vulkan initializtions -------------------- >

    void Application::create_and_show_window() {
        // Initialize the GLFW library.
        glfwInit();

        // Tell GLFW not to create an OpenGL context.
        // Because DUHHH!! We're doing Vulkan here.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // We'll handle window resizing later.
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Create the GLFW Window.
        m_ptr_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
            WINDOW_TITLE, nullptr, nullptr);

        VK_TUT_LOG_DEBUG("Created and showed the window.");
    }

    void Application::init_vulkan_instance() {
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        if (!check_validation_layer_support()) {
            VK_TUT_LOG_ERROR(
                "Validation layers requested, but not available."
            );
        }
#endif
        // Technically optional but useful information
        // for the graphics driver to optimize our application.
        VkApplicationInfo vulkan_application_info{};
        vulkan_application_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vulkan_application_info.pApplicationName = WINDOW_TITLE;
        vulkan_application_info.pEngineName = "No Engine";
        vulkan_application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        vulkan_application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        vulkan_application_info.apiVersion = VK_API_VERSION_1_3;

        // Retrieve the number of glfw extensions.
        uint32_t glfw_extensions_count = 0;
        const char** glfw_extensions =
            glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
        // Store the extension names to the required extensions.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        ::std::vector<const char*> required_extensions(
            glfw_extensions_count + 1
        );
        required_extensions.assign(glfw_extensions_count,
            *glfw_extensions);
        required_extensions.emplace_back(
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
// When validation layer is not enabled.
#else
        ::std::vector<const char*> required_extensions(
            glfw_extensions,
            glfw_extensions + glfw_extensions_count
        );
#endif

        // Debug messanger for the vulkan instance creation.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        VkDebugUtilsMessengerCreateInfoEXT
            vulkan_instance_creation_debug_messanger_info{};
        populate_debug_utils_messanger_info(
            vulkan_instance_creation_debug_messanger_info);
#endif

        // Information about the vulkan instance to be created.
        VkInstanceCreateInfo vulkan_instance_info{};
        vulkan_instance_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vulkan_instance_info.pApplicationInfo = &vulkan_application_info;
        vulkan_instance_info.enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size());
        vulkan_instance_info.ppEnabledExtensionNames =
            required_extensions.data();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        vulkan_instance_info.enabledLayerCount =
            static_cast<uint32_t>(m_enabled_layers_names.size());
        vulkan_instance_info.ppEnabledLayerNames =
            m_enabled_layers_names.data();
        vulkan_instance_info.pNext =
            &vulkan_instance_creation_debug_messanger_info;
#endif

        // Create the Vulkan instance.
        if (vkCreateInstance(&vulkan_instance_info, nullptr,
        &m_vulkan_instance) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create vulkan instance.");
        }

        VK_TUT_LOG_DEBUG("Successfully created a Vulkan instance.");
    }

    void Application::select_physical_devices() {
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
            QueueFamilyIndices indices = find_family_indices(physical_device);
            bool physical_device_suitable = indices.is_complete();

            // Map the device if suitable.
            if (physical_device_suitable) {
                // The corresponding logical device for the physical device.
                VkDevice logical_device;
                // Place in the map.
                m_device_map.emplace(physical_device, logical_device);
            }
        }

        if (m_device_map.size() == 0) {
            VK_TUT_LOG_ERROR(
                "Failed to find a suitable physical device."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully found suitable physical devices.");
    }

    void Application::create_logical_devices() {
        // Loop through the device map and create the
        // logical device for each physical device.
        for (auto& [physical_device, logical_device] :
        m_device_map) {
            QueueFamilyIndices indices = find_family_indices(physical_device);
            if (!indices.is_complete()) {
                // If an unsuitable device somehow got through.
                VK_TUT_LOG_ERROR("Unsuitable physical device.");
            }

            // Information about the device queues to be made.
            VkDeviceQueueCreateInfo device_queue_info{};
            device_queue_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_info.queueFamilyIndex = ::std::get<1>(
                indices.get_graphics_family_index());
            device_queue_info.queueCount = 1;
            float queue_priority = 1.0f;
            device_queue_info.pQueuePriorities = &queue_priority;

            // Infomration about the device features to be enabled.
            VkPhysicalDeviceFeatures enabled_device_features{};
            
            // Information about the logical device.
            VkDeviceCreateInfo logical_device_info{};
            logical_device_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            logical_device_info.queueCreateInfoCount = 1;
            logical_device_info.pQueueCreateInfos = &device_queue_info;
            logical_device_info.pEnabledFeatures = &enabled_device_features;
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
            logical_device_info.enabledLayerCount =
                static_cast<uint32_t>(m_enabled_layers_names.size());
            logical_device_info.ppEnabledLayerNames =
                m_enabled_layers_names.data();
#endif

            if(vkCreateDevice(physical_device, &logical_device_info,
            nullptr, &logical_device) != VK_SUCCESS) {
                VK_TUT_LOG_ERROR(
                    "Failed to create a logical device."
                );
            }
        }

        VK_TUT_LOG_DEBUG("Successfully created logical devices.");
    }

    // < ------------------ END Vulkan initializtions ------------------ >

    // < ------------------- Vulkan cleanup functions ------------------ >

    void Application::destroy_logical_devices() {
        // Loop through the device map and destroy the
        // logical device for each physical device.
        for (auto& [physical_device, logical_device]:
        m_device_map) {
            vkDestroyDevice(logical_device, nullptr);
        }

        VK_TUT_LOG_DEBUG("Destroyed logical devices.");
    }

    void Application::destroy_vulkan_instance() {
        // Destroy the vulkan instance handle.
        vkDestroyInstance(m_vulkan_instance, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed the Vulkan Instance.");
    }

    void Application::destroy_window() {
        // Destroy the GLFW window that was created.
        glfwDestroyWindow(m_ptr_window);
        // Terminate the GLFW library.
        glfwTerminate();

        VK_TUT_LOG_DEBUG("Destroyed the GLFW Window and terminated GLFW.");
    }

    // < ----------------- END Vulkan cleanup functions ---------------- >

    // < --------------- Validation layer initializations -------------- >

// These functions will be declared and defined if and
// only if _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

    bool Application::check_validation_layer_support() {
        uint32_t available_layers_properties_count;
        vkEnumerateInstanceLayerProperties(
            &available_layers_properties_count, nullptr);

        std::vector<VkLayerProperties> available_layers_properties(
            available_layers_properties_count);
        vkEnumerateInstanceLayerProperties(&available_layers_properties_count,
            available_layers_properties.data());
        
        for (const VkLayerProperties& layer_prop : available_layers_properties) {
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

    void Application::setup_debug_messanger() {
        VkDebugUtilsMessengerCreateInfoEXT debug_messanger_info{};
        populate_debug_utils_messanger_info(debug_messanger_info);

        if (create_debug_utils_messengerEXT(m_vulkan_instance,
        &debug_messanger_info, nullptr, &m_debug_messanger) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create debug messanger."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created debug messanger.");
    }

#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

    // < ------------- END Validation layer initializations ------------ >

    // < -------------- Validation layer helper functions -------------- >

// These functions will be declared and defined if and
// only if _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

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
            return VK_ERROR_EXTENSION_NOT_PRESENT;
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
            VK_TUT_LOG_DEBUG("Destroyed the debug messanger.");
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* ptr_callback_data,
        void* ptr_user_data
    ) {
        ::std::cerr << "\033[0;92m[" << "VULKAN"
            << "]\033[0m " << ptr_callback_data->pMessage << "\n";

        return VK_FALSE;
    }

    void populate_debug_utils_messanger_info(
        VkDebugUtilsMessengerCreateInfoEXT& ref_debug_messanger_info
    ) {
        ref_debug_messanger_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        ref_debug_messanger_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        ref_debug_messanger_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        ref_debug_messanger_info.pfnUserCallback = debug_callback;
    }

#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

    // < ------------ END Validation layer helper functions ------------ >
}