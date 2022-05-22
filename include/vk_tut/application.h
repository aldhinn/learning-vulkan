#if !defined(_VK_TUT_APPLICATION_HEADER_)
#define _VK_TUT_APPLICATION_HEADER_

// C++ only region.
#if defined(__cplusplus)

#include <GLFW/glfw3.h>

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

        // The pointer to the GLFW window object.
        GLFWwindow* m_ptr_window;

        // < -------------------- Vulkan initializtions -------------------- >

        void create_and_show_window();

        // < ------------------ END Vulkan initializtions ------------------ >
    };
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.