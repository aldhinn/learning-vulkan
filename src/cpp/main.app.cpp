#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#include <iostream>
#include <exception>

// Executable entry point.
int main(int argc, char** argv) {
    try {
        VK_TUT_LOG_TRACE("Creating Application Instance.");
        ::vk::tut::Application app;
        app.run();
    }
    catch(const ::std::exception& ex) {
        VK_TUT_LOG_TRACE("Exiting with code EXIT_FAILURE(1)");
        return EXIT_FAILURE;
    }

    VK_TUT_LOG_TRACE("Exiting with code EXIT_SUCCESS(0)");
    return EXIT_SUCCESS;
}