cmake_minimum_required(VERSION 3.22)
project(learning_vulkan)

include(FetchContent)

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

FetchContent_Declare(
    googletest
    GIT_REPOSITORY      https://github.com/google/googletest.git
    GIT_TAG             v1.15.2
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY      https://github.com/glfw/glfw.git
    GIT_TAG             3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY      https://github.com/g-truc/glm.git
    GIT_TAG             1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    stb
    GIT_REPOSITORY      https://github.com/nothings/stb.git
    GIT_TAG             master
)
FetchContent_MakeAvailable(stb)

# < --------------------------- END Dependencies -------------------------- >

# < -------------------------- Compiling Shaders -------------------------- >

# This program comes with the Vulkan SDK
find_program(GLSLC_EXE glslc REQUIRED)
# Ensure output directory's existence.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/)
# Retrieve shader source files.
file(GLOB shader_source_filepaths
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glsl/*.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glsl/*.frag
)
# Loop through each source file and compile to a shader binary format.
foreach(source_filepath ${shader_source_filepaths})
    # Retrieve the base filename of the source file.
    get_filename_component(source_filename ${source_filepath} NAME)
    # Execute shader compilation.
    exec_program(${GLSLC_EXE} ARGS
        ${source_filepath} -o
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/${source_filename}.spv
        -O
        --target-env=vulkan1.2
    )
endforeach()

# < ------------------------ END Compiling Shaders ------------------------ >

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
    ${stb_SOURCE_DIR}
)
target_link_libraries(
    learning_vulkan_lib PUBLIC
    ${Vulkan_LIBRARIES}
    glfw
    glm
)
target_compile_definitions(
    learning_vulkan_lib PUBLIC
    _VK_TUT_VERTEX_SHADER_FILEPATH_="${CMAKE_CURRENT_BINARY_DIR}/shaders/basic_shader.vert.spv"
    _VK_TUT_FRAGMENT_SHADER_FILEPATH_="${CMAKE_CURRENT_BINARY_DIR}/shaders/basic_shader.frag.spv"
    _VK_TUT_TEXTURE_PATH_="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/texture.jpg"
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

enable_testing()

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

# Add to CTest.
add_test(NAME "Unit Testing" COMMAND learning_vulkan_unittests)

# < --------------------------- END Unit testing -------------------------- >