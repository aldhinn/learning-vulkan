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

find_package(Vulkan REQUIRED)
find_package(GTest)
if (NOT GTest_FOUND)
    # Will add the submodule for googletest if not installed in the system.  
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/)
endif()
find_package(glfw3)
if (NOT glfw3_FOUND)
    # Will add the submodule for glfw if not installed in the system.
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/)
endif()
find_package(glm)
if (NOT glm_FOUND)
    # Will add the submodule for glm if not installed in the system.
    set(GLM_TEST_ENABLE OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm/)
endif()

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
    ${Vulkan_INCLUDE_DIR}
)
target_link_libraries(
    learning_vulkan_lib PUBLIC
    ${Vulkan_LIBRARIES}
    glfw
    glm::glm
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
target_compile_definitions(
    learning_vulkan_unittests PRIVATE
    _VK_TUT_PROJECT_DIR_="${CMAKE_CURRENT_SOURCE_DIR}"
)

# < --------------------------- END Unit testing -------------------------- >