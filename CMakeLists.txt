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
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/
)

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
)
target_link_libraries(
    learning_vulkan_lib PUBLIC
    ${Vulkan_LIBRARIES}
    glfw
    glm::glm
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