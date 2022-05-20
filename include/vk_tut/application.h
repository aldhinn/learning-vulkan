#if !defined(_VK_TUT_APPLICATION_HEADER_)
#define _VK_TUT_APPLICATION_HEADER_

// C++ only region.
#if defined(__cplusplus)

namespace vk::tut {
    // Vulkan application data encapsulation.
    class Application {
    public:
        // Prevent copying.
        inline Application(const Application&) = delete;
        // Prevent moving.
        inline Application(Application&&) = delete;
        // Prevent copy re-assignment.
        inline Application& operator=(const Application&) = delete;
        // Prevent move re-assignment.
        inline Application& operator=(Application&&) = delete;
    };
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.