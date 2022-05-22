#include <vulkan/vulkan.h>
#include <gtest/gtest.h>
#include <GLFW/glfw3.h>

#include "vk_tut/logging.h"

namespace vk::tut {
    using ::std::cout;

    // Vulkan API Test Fixture.
    class VulkanApiTests : public ::testing::Test {
    protected:
        // The handle to the Vulkan library.
        VkInstance m_vulkan_instance;

        // Runs before each test.
        inline void SetUp() override {
            // Information about the vulkan instance to be created.
            VkInstanceCreateInfo vulkan_instance_info{};
            vulkan_instance_info.sType = VkStructureType
                ::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

            // Create the Vulkan instance.
            if (vkCreateInstance(&vulkan_instance_info, nullptr,
            &m_vulkan_instance) != VK_SUCCESS) {
                VK_TUT_LOG_ERROR("Failed to create vulkan instance.");
            }
            cout << "\n";
        }
        // Runs after each test.
        inline void TearDown() override {
            // Destroy the vulkan instance handle.
            vkDestroyInstance(m_vulkan_instance, nullptr);
            cout << "\n";
        }
    };

    TEST_F(VulkanApiTests, print_glfw_extensions) {
        // Initialize the GLFW library.
        glfwInit();

        // Retrieve the number of glfw extensions.
        uint32_t glfw_extensions_count = 0;
        const char** glfw_extensions_char_ptr_ptr =
            glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
        // Store the extension names to a const char ptr vector.
        ::std::vector<const char*> glfw_extensions(
            glfw_extensions_char_ptr_ptr,
            glfw_extensions_char_ptr_ptr + glfw_extensions_count
        );

        cout << "\033[0;96m" << "GLFW Extensions Needed:" << "\033[0m\n";

        for (const char* extension : glfw_extensions) {
            cout << "\033[0;95m" << extension << "\033[0m\n";
        }

        // Terminate the GLFW library.
        glfwTerminate();
    }
}