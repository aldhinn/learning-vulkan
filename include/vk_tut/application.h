#if !defined(_VK_TUT_APPLICATION_HEADER_)
#define _VK_TUT_APPLICATION_HEADER_

// Enable validation layer in non-release modes.
#if !defined(NDEBUG) && !defined(_NDEBUG)
#if !defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
#define _VK_TUT_VALIDATION_LAYER_ENABLED_
#endif
#else // Release mode.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
// Undefine this macro if it was somehow defined.
#undef _VK_TUT_VALIDATION_LAYER_ENABLED_
#endif
#endif

// C++ only region.
#if defined(__cplusplus)

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>

namespace vk::tut {
    // Vulkan application data encapsulation.
    class Application final {
    public:
        // Default constructor.
        Application();
        // Code cleanup.
        ~Application();

        // Runs the application loop.
        void run();

        // Prevent copying.
        inline constexpr Application(const Application&) = delete;
        // Prevent moving.
        inline constexpr Application(Application&&) = delete;
        // Prevent copy re-assignment.
        inline constexpr Application& operator=(const Application&) = delete;
        // Prevent move re-assignment.
        inline constexpr Application& operator=(Application&&) = delete;
    
    private:
        // GLFW Window constants.
        const uint32_t WINDOW_WIDTH = 900, WINDOW_HEIGHT = 600;
        const char* WINDOW_TITLE = "Vulkan Tutorial Sandbox";

        // The GLFW window handle.
        GLFWwindow* m_ptr_window;
        // The handle to the Vulkan library.
        VkInstance m_vulkan_instance;
        // The list of enabled layers.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        const ::std::vector<const char*> m_enabled_layers_names = {
            "VK_LAYER_KHRONOS_validation"
        };
#endif
        // The debug messanger.
        // Only declared and defined when
        // _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        VkDebugUtilsMessengerEXT m_debug_messanger;
#endif

        // < -------------------- Vulkan initializtions -------------------- >

        void create_and_show_window();
        void init_vulkan_instance();

        // < ------------------ END Vulkan initializtions ------------------ >

        // < --------------- Validation layer initializations -------------- >

// These functions will be declared and defined if and
// only if _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        bool check_validation_layer_support();
        void setup_debug_messanger();
#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

        // < ------------- END Validation layer initializations ------------ >
    };

    // < -------------- Validation layer helper functions -------------- >

// These functions will be declared and defined if and
// only if _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
    VkResult create_debug_utils_messengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* ptr_create_info,
        const VkAllocationCallbacks* ptr_allocator,
        VkDebugUtilsMessengerEXT* ptr_debug_messenger
    );
    void destroy_debug_utils_messengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debug_messenger,
        const VkAllocationCallbacks* ptr_allocator
    );
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* ptr_callback_data,
        void* ptr_user_data
    );
    void populate_debug_utils_messanger_info(
        VkDebugUtilsMessengerCreateInfoEXT& ref_debug_messanger_info
    );
#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

    // < ------------ END Validation layer helper functions ------------ >
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.