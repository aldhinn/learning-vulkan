#include "vk_tut/logging.h"

#include <gtest/gtest.h>

namespace vk::tut {
    using ::std::cout;

    // Logging Test Fixture.
    class LoggingTests : public ::testing::Test {
    protected:
        // Runs before each test.
        inline void SetUp() override {
            cout << "\n";
        }
        // Runs after each test.
        inline void TearDown() override {
            cout << "\n";
        }
    };

    TEST_F(LoggingTests, general_logs) {
        VK_TUT_LOG(DEMO, \033[0;95m, ::std::cout, "Hello from general_logs test");
    }

    TEST_F(LoggingTests, trace_logs) {
        VK_TUT_LOG_TRACE("Hello from trace.");
        VK_TUT_LOG_TRACE("Another trace message.");
    }

    TEST_F(LoggingTests, debug_logs) {
        VK_TUT_LOG_DEBUG("Hello from debug.");
        VK_TUT_LOG_DEBUG("Another debug message.");
    }

    TEST_F(LoggingTests, error_logs) {
        char exception_detector = 0b00000000;

        try {
            VK_TUT_LOG_ERROR("Hello from error.");
        }
        catch(const ::std::exception& ex) {
            cout << "Execption thrown: \"" << ex.what() << "\"\n";
            exception_detector = exception_detector | 0b00000001;
        }

        try {
            VK_TUT_LOG_ERROR("Another error message.");
        }
        catch(const ::std::exception& ex) {
            cout << "Execption thrown: \"" << ex.what() << "\"\n";
            exception_detector = exception_detector | 0b00000010;
        }

        if (exception_detector != 0b00000011) {
            throw ::std::runtime_error(
                "VK_TUT_LOG_ERROR didn't throw an error at some point."
            );
        }
    }
}