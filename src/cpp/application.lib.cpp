#include "vk_tut/application.h"
#include "vk_tut/logging.h"
#include "vk_tut/SwapChainSupportDetails.h"
#include "vk_tut/QueueFamilyIndices.h"

#include <cstring>
#include <tuple>
#include <set>
#include <string>
#include <stdint.h>
#include <limits>
#include <algorithm>

namespace vk::tut {
    // Default constructor.
    Application::Application() {
        VK_TUT_LOG_DEBUG("...Initializing application data...");

        create_and_show_window();
        init_vulkan_instance();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        setup_debug_messenger();
#endif
        create_surface();
        select_physical_device();
        create_logical_device();
        create_swapchain();

        VK_TUT_LOG_DEBUG("...FINISHED Initializing application data...");
    }

    // Code cleanup.
    Application::~Application() {
        VK_TUT_LOG_DEBUG("...Cleaning up application data...");

        destroy_swapchain();
        destroy_logical_device();
        destroy_surface();
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        destroy_debug_utils_messengerEXT(m_vulkan_instance,
            m_debug_messenger, nullptr);
#endif
        destroy_vulkan_instance();
        destroy_window();

        VK_TUT_LOG_DEBUG("...FINISHED cleaning up application data...");
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

    // < -------------------- Vulkan initializations ------------------- >

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
        ::std::vector<const char*> required_extensions;
        required_extensions.reserve(glfw_extensions_count + 1);
        for (const char* extension :
        ::std::vector<const char*>(glfw_extensions,
        glfw_extensions + glfw_extensions_count)) {
            required_extensions.emplace_back(extension);
        }
        required_extensions.emplace_back(
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // Print the names of the required extensions.
        VK_TUT_LOG_TRACE("Tracing value of required extensions...");
        for (const char* extension : required_extensions) {
            VK_TUT_LOG_TRACE(extension);
        }
// When validation layer is not enabled.
#else
        ::std::vector<const char*> required_extensions(
            glfw_extensions,
            glfw_extensions + glfw_extensions_count
        );
#endif

        // Debug messenger for the vulkan instance creation.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        VkDebugUtilsMessengerCreateInfoEXT
            vulkan_instance_creation_debug_messenger_info{};
        populate_debug_utils_messenger_info(
            vulkan_instance_creation_debug_messenger_info);
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
            &vulkan_instance_creation_debug_messenger_info;
#endif

        // Create the Vulkan instance.
        if (vkCreateInstance(&vulkan_instance_info, nullptr,
        &m_vulkan_instance) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create vulkan instance.");
        }

        VK_TUT_LOG_DEBUG("Successfully created a Vulkan instance.");
    }

    void Application::create_surface() {
        if (glfwCreateWindowSurface(m_vulkan_instance,
        m_ptr_window, nullptr, &m_surface) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR("Failed to create a surface.");
        }

        VK_TUT_LOG_DEBUG("Successfully created a surface.");
    }

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
        nullptr, &m_logical_device) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create a logical device."
            );
        }

        // Retrieve the present queue handle.
        vkGetDeviceQueue(m_logical_device,
            ::std::get<1>(indices.get_present_family_index()),
            0, &m_present_queue
        );

        VK_TUT_LOG_DEBUG("Successfully created a logical device.");
    }

    void Application::create_swapchain() {
        SwapChainSupportDetails swapchan_support = query_swapchain_support(
            m_physical_device, m_surface
        );

        VkSurfaceFormatKHR surface_format = choose_surface_format(
            swapchan_support.get_formats()  
        );
        m_swapchain_image_format = surface_format.format;
        VkPresentModeKHR present_mode = choose_present_mode(
            swapchan_support.get_present_modes()
        );
        m_swapchain_extent = choose_swap_extent(
            swapchan_support.get_capabilities(), m_ptr_window
        );

        uint32_t image_count = swapchan_support
            .get_capabilities().minImageCount + 1;
        // Reference for ::std::clamp
        // https://en.cppreference.com/w/cpp/algorithm/clamp
        // Basically keeping the values
        // of width and height within valid range.
        image_count = ::std::clamp(image_count,
            swapchan_support.get_capabilities().minImageCount,
            swapchan_support.get_capabilities().maxImageCount > 0 ?
                swapchan_support.get_capabilities().maxImageCount :
                ::std::numeric_limits<uint32_t>::max()
        );

        QueueFamilyIndices indices = find_family_indices(
            m_physical_device, m_surface
        );

        // If an invalid physical device somehow got through.
        if (!indices.is_complete()) {
            VK_TUT_LOG_ERROR(
                "Invalid physical device for swapchain creation."
            );
        }

        uint32_t queue_family_indices [] = {
            ::std::get<1>(indices.get_graphics_family_index()),
            ::std::get<1>(indices.get_present_family_index())
        };

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VkStructureType
            ::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = m_surface;
        swapchain_info.minImageCount = image_count;
        swapchain_info.imageFormat = m_swapchain_image_format;
        swapchain_info.presentMode = present_mode;
        swapchain_info.clipped = VK_TRUE; // Simply clip the obscured pixels.
        swapchain_info.imageExtent = m_swapchain_extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VkImageUsageFlagBits
            ::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.preTransform = swapchan_support
            .get_capabilities().currentTransform;
        // Used for blending with other windows in the
        // windows system. Perhaps, to create some tranclucency
        // effect on the window. We'll set it to opaque for now.
        swapchain_info.compositeAlpha = VkCompositeAlphaFlagBitsKHR
            ::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        if (queue_family_indices[0] != queue_family_indices[1]) {
            swapchain_info.imageSharingMode = VkSharingMode
                ::VK_SHARING_MODE_CONCURRENT;   
            swapchain_info.queueFamilyIndexCount = 2;
            swapchain_info.pQueueFamilyIndices = queue_family_indices;
        }
        else {
            swapchain_info.imageSharingMode = VkSharingMode
                ::VK_SHARING_MODE_EXCLUSIVE;
        }
        
        if (vkCreateSwapchainKHR(m_logical_device, &swapchain_info,
        nullptr, &m_swapchain) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create a swapchain."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created swapchain.");

        // Retrieve swapchain images.
        vkGetSwapchainImagesKHR(m_logical_device, m_swapchain,
            &image_count, nullptr);
        m_swapchain_images.reserve(image_count);
        vkGetSwapchainImagesKHR(m_logical_device, m_swapchain,
            &image_count, m_swapchain_images.data());
    }

    // < ------------------ END Vulkan initializations ----------------- >

    // < ------------------- Vulkan cleanup functions ------------------ >

    void Application::destroy_swapchain() {
        vkDestroySwapchainKHR(m_logical_device, m_swapchain, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed swapchain.");
    }

    void Application::destroy_logical_device() {
        vkDestroyDevice(m_logical_device, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed logical device.");
    }

    void Application::destroy_surface() {
        vkDestroySurfaceKHR(m_vulkan_instance, m_surface, nullptr);

        VK_TUT_LOG_DEBUG("Destroyed surface.");
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

    // < ----------------------- Helper functions ---------------------- >

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

    VkSurfaceFormatKHR choose_surface_format(
        const ::std::vector<VkSurfaceFormatKHR>& formats
    ) {
        for (const VkSurfaceFormatKHR format : formats) {
            if (format.format == VkFormat::VK_FORMAT_B8G8R8_SRGB &&
            format.colorSpace == VkColorSpaceKHR
            ::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            ) return format;
        }

        // Just settle on the first one if the formats aren't perfect.
        return formats[0];
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

    // This dictates the resoultion of the swapchain images.
    VkExtent2D choose_swap_extent(
        const VkSurfaceCapabilitiesKHR& capabilities,
        GLFWwindow* ptr_window
    ) {
        if (capabilities.currentExtent.width !=
        ::std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
        
        int width, height;
        glfwGetFramebufferSize(ptr_window, &width, &height);

        VkExtent2D actual_extent{
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        // Reference for ::std::clamp
        // https://en.cppreference.com/w/cpp/algorithm/clamp
        // Basically keeping the values
        // of width and height within valid range.
        actual_extent.width = ::std::clamp(actual_extent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );
        actual_extent.height = ::std::clamp(actual_extent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );

        return actual_extent;
    }

    // < --------------------- END Helper functions -------------------- >

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
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info{};
        populate_debug_utils_messenger_info(debug_messenger_info);

        if (create_debug_utils_messengerEXT(m_vulkan_instance,
        &debug_messenger_info, nullptr, &m_debug_messenger) != VK_SUCCESS) {
            VK_TUT_LOG_ERROR(
                "Failed to create debug messenger."
            );
        }

        VK_TUT_LOG_DEBUG("Successfully created debug messenger.");
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
            VK_TUT_LOG_DEBUG("Destroyed the debug messenger.");
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

#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

    // < ------------ END Validation layer helper functions ------------ >
}