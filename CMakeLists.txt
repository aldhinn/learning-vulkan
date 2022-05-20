cmake_minimum_required(VERSION 3.22)
project(learning_vulkan)

# < ---------------------- Enforcing C++ 20 standard ---------------------- >

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
else()
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# < -------------------- END Enforcing C++ 20 standard -------------------- >

# < ----------------------------- Dependencies ---------------------------- >

find_package(GTest REQUIRED)
find_package(Vulkan REQUIRED)

# < --------------------------- END Dependencies -------------------------- >

# < ---------------- learning_vulkan_lib target definition ---------------- >

file(GLOB learning_vulkan_lib_src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/*.lib.cpp
)
add_library(
    learning_vulkan_lib
    ${learning_vulkan_lib_src}
)
target_include_directories(
    learning_vulkan_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
target_link_libraries(
    learning_vulkan_lib PUBLIC
    vulkan
)

# < -------------- END learning_vulkan_lib target definition -------------- >

# < ---------------- learning_vulkan_app target definition ---------------- >

file(GLOB learning_vulkan_app_src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/*.app.cpp
)
add_executable(
    learning_vulkan_app
    ${learning_vulkan_app_src}
)
target_link_libraries(
    learning_vulkan_app PUBLIC
    learning_vulkan_lib
)

# < -------------- END learning_vulkan_app target definition -------------- >

# < ----------------------------- Unit testing ---------------------------- >

file(GLOB learning_vulkan_unittests_src
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/*.UT.cpp
)
add_executable(
    learning_vulkan_unittests
    ${learning_vulkan_unittests_src}
)
target_link_libraries(
    learning_vulkan_unittests PUBLIC
    GTest::gtest_main
    learning_vulkan_lib
)

# < --------------------------- END Unit testing -------------------------- >