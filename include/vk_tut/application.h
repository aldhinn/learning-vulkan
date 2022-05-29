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

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
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
        // The debug messenger.
        // Only declared and defined when
        // _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined.
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
        // The abstract surface to present render graphics to.
        VkSurfaceKHR m_surface;
        // The handle to the physical GPU.
        VkPhysicalDevice m_physical_device;
        // The logical device used to interface with the physical device.
        VkDevice m_logical_device;
        // The presentation queue
        VkQueue m_present_queue;
        // List of enabled device extensions.
        const ::std::vector<const char*> m_enabled_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        // The swapchain handle.
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        VkExtent2D m_swapchain_extent;
        // The handles to the swapchain images.
        ::std::vector<VkImage> m_swapchain_images;
        // The handles to the swapchain image views.
        ::std::vector<VkImageView> m_swapchain_image_views;
        // Vertex shader module.
        VkShaderModule m_vertex_shader_module;
        // Fragment shader module.
        VkShaderModule m_fragment_shader_module;
        // The render pass handle.
        VkRenderPass m_render_pass;
        // The graphics pipeline layout.
        VkPipelineLayout m_graphics_pipeline_layout;
        // The handle to the graphics pipeline.
        VkPipeline m_graphics_pipeline;
        // The handles to the frame buffers.
        ::std::vector<VkFramebuffer> m_swapchain_frame_buffers;

        // < -------------------- Vulkan initializations ------------------- >

        void create_and_show_window();
        void init_vulkan_instance();
        void create_surface();
        void select_physical_device();
        void create_logical_device();
        void create_swapchain();
        void create_swapchain_image_views();
        void create_render_pass();
        void create_graphics_pipeline();
        void create_swapchain_frame_buffers();

        // < ------------------ END Vulkan initializations ----------------- >

        // < ------------------- Vulkan cleanup functions ------------------ >

        void destroy_swapchain_frame_buffers();
        void destroy_graphics_pipeline();
        void destroy_render_pass();
        void destroy_swapchain_image_views();
        void destroy_swapchain();
        void destroy_logical_device();
        void destroy_surface();
        void destroy_vulkan_instance();
        void destroy_window();

        // < ----------------- END Vulkan cleanup functions ---------------- >

        // < --------------- Validation layer initializations -------------- >

// These functions will be declared and defined if and
// only if _VK_TUT_VALIDATION_LAYER_ENABLED_ is defined
#if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)
        bool check_validation_layer_support();
        void setup_debug_messenger();
#endif
// End #if defined(_VK_TUT_VALIDATION_LAYER_ENABLED_)

        // < ------------- END Validation layer initializations ------------ >
    };

    // < ----------------------- Helper functions ---------------------- >

    bool check_device_extension_support(
        const VkPhysicalDevice& physical_device,
        const ::std::vector<const char*>& required_extensions
    );
    VkSurfaceFormatKHR choose_surface_format(
        const ::std::vector<VkSurfaceFormatKHR>& formats
    );
    VkPresentModeKHR choose_present_mode(
        const ::std::vector<VkPresentModeKHR>& present_modes
    );
    // This dictates the resoultion of the swapchain images.
    VkExtent2D choose_swap_extent(
        const VkSurfaceCapabilitiesKHR& capabilities,
        GLFWwindow* ptr_window
    );

    // < --------------------- END Helper functions -------------------- >

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
    void populate_debug_utils_messenger_info(
        VkDebugUtilsMessengerCreateInfoEXT& ref_debug_messenger_info
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