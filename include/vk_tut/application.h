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

#include "vk_tut/vertex.h"

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
        // The graphics queue handle.
        VkQueue m_graphics_queue;
        // List of enabled device extensions.
        const ::std::vector<const char*> m_enabled_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        // The swapchain handle.
        VkSwapchainKHR m_swapchain;
        // The format of the images in the swapchain.
        VkFormat m_swapchain_image_format;
        // The extent description of the swapchain.
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
        // The descriptor layout handle.
        VkDescriptorSetLayout m_descriptor_set_layout;
        // The graphics pipeline layout.
        VkPipelineLayout m_graphics_pipeline_layout;
        // The handle to the graphics pipeline.
        VkPipeline m_graphics_pipeline;
        // The handles to the frame buffers.
        ::std::vector<VkFramebuffer> m_swapchain_frame_buffers;
        // The command pool handle.
        VkCommandPool m_command_pool;
        // The handle to the texture image.
        VkImage m_texture_image;
        // The image view of the texture image.
        VkImageView m_texture_image_view;
        // The sample of the texture image.
        VkSampler m_texture_sampler;
        // The handle to the texture image device memory.
        VkDeviceMemory m_texture_image_memory;
        // The value of the vertices of the object to be rendered.
        ::std::vector<Vertex> m_vertices;
        // The value of the index buffers of the object to be rendered.
        ::std::vector<uint32_t> m_indices;
        // The handle to the buffer containing vertex and index data
        // of all meshes loaded to be rendered..
        VkBuffer m_mesh_buffer;
        // The handle to the memory of the mesh buffer in the GPU.
        VkDeviceMemory m_mesh_buffer_memory;
        // The handles to the buffer containing uniform data.
        ::std::vector<VkBuffer> m_uniform_buffers;
        // The handles to the memory of the uniform buffer in the GPU.
        ::std::vector<VkDeviceMemory> m_uniform_buffer_memories;
        // The descriptor pool handle.
        VkDescriptorPool m_descriptor_pool;
        // The handles to the descriptor sets.
        ::std::vector<VkDescriptorSet> m_descriptor_sets;
        // The index of the current frame being rendered.
        uint32_t m_current_frame_index = 0;
        // The command buffer handles.
        ::std::vector<VkCommandBuffer> m_command_buffers;
        // GPU sync objects that signals a swapchain
        // image availability in the GPU.
        ::std::vector<VkSemaphore> m_image_available_semaphores;
        // GPU Sync objects that signals the
        // completion of rendering in the GPU.
        ::std::vector<VkSemaphore> m_render_finished_semaphores;
        // CPU sync objects that tells the CPU that the
        // previous frame has finished rendering in the GPU.
        ::std::vector<VkFence> m_in_flight_fences;

        // < -------------------- Vulkan initializations ------------------- >

        void create_and_show_window();
        void init_vulkan_instance();
        void create_surface();
        void select_physical_device();
        void create_logical_device();
        void create_swapchain();
        void create_swapchain_image_views();
        void create_render_pass();
        void create_descriptor_set_layout();
        void create_graphics_pipeline();
        void create_swapchain_frame_buffers();
        void create_command_pool();
        void create_texture_image();
        void create_texture_image_view();
        void create_texture_sampler();
        void create_mesh_buffer();
        void create_uniform_buffers();
        void create_descriptor_pool();
        void create_descriptor_sets();
        void create_command_buffers();
        void create_sync_objects();

        // < ------------------ END Vulkan initializations ----------------- >

        // < ------------------- Vulkan cleanup functions ------------------ >

        void destroy_sync_objects();
        void destroy_descriptor_pool();
        void destroy_uniform_buffers();
        void destroy_mesh_buffer();
        void destroy_texture_sampler();
        void destroy_texture_image_view();
        void destroy_texture_image();
        void destroy_command_pool();
        void destroy_swapchain_frame_buffers();
        void destroy_graphics_pipeline();
        void destroy_descriptor_set_layout();
        void destroy_render_pass();
        void destroy_swapchain_image_views();
        void destroy_swapchain();
        void destroy_logical_device();
        void destroy_surface();
        void destroy_vulkan_instance();
        void destroy_window();

        // < ----------------- END Vulkan cleanup functions ---------------- >

        // < ---------------------------- Jobs ----------------------------- >

        void record_command_buffer(
            const uint32_t& image_index
        );
        void draw_frame();
        void recreate_swapchain();
        void update_uniform_buffer();
        void load_initial_mesh();
        void load_square_mesh();

        // < -------------------------- END Jobs --------------------------- >

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
    uint32_t find_memory_requirements(
        const VkPhysicalDevice& physical_device,
        const uint32_t& type_filter,
        const VkMemoryPropertyFlags& properties
    );
    VkCommandBuffer begin_single_time_commands(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool
    );
    void end_single_time_commands(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkCommandBuffer& command_buffer
    );
    void create_and_allocate_buffer(
        const VkPhysicalDevice& physical_device,
        const VkDevice& logical_device,
        const VkDeviceSize& device_size,
        const VkBufferUsageFlags& usage_flags,
        const VkMemoryPropertyFlags& memory_properties,
        VkBuffer* ptr_buffer,
        VkDeviceMemory* ptr_buffer_memory
    );
    void copy_buffer(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkBuffer& src_buffer,
        const VkBuffer& dest_buffer,
        const VkDeviceSize& buffer_size
    );
    void create_and_allocate_image(
        const VkPhysicalDevice& physical_device,
        const VkDevice& logical_device,
        const int& width, const int& height,
        const VkFormat& format, const VkImageTiling& tiling,
        const VkImageUsageFlags& usage,
        const VkMemoryPropertyFlags& memory_properties,
        VkImage* ptr_image,
        VkDeviceMemory* ptr_image_memory
    );
    void copy_buffer_to_image(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const uint32_t& width, const uint32_t& height,
        const VkBuffer& src_buffer, const VkImage& dst_image
    );
    void transition_image_layout(
        const VkDevice& logical_device,
        const VkCommandPool& command_pool,
        const VkQueue& queue,
        const VkImageLayout& old_layout,
        const VkImageLayout& new_layout,
        const VkImage& image
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