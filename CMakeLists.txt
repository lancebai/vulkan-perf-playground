cmake_minimum_required(VERSION 3.22)
project(vulkan_app CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure local X11 dev paths (downloaded dev libraries)
set(X11_DEV_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/x11_dev/usr")
if(EXISTS "${X11_DEV_PATH}")
    message(STATUS "Using local X11 dev libraries at ${X11_DEV_PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${X11_DEV_PATH}")
    list(APPEND CMAKE_INCLUDE_PATH "${X11_DEV_PATH}/include")
    list(APPEND CMAKE_LIBRARY_PATH "${X11_DEV_PATH}/lib/x86_64-linux-gnu")
    include_directories(SYSTEM "${X11_DEV_PATH}/include")
    link_directories("${X11_DEV_PATH}/lib/x86_64-linux-gnu")
endif()

# Configure Vulkan SDK path
set(VULKAN_SDK "${CMAKE_CURRENT_SOURCE_DIR}/vulkansdk/x86_64")
if(EXISTS "${VULKAN_SDK}")
    message(STATUS "Using local Vulkan SDK at ${VULKAN_SDK}")
    set(ENV{VULKAN_SDK} "${VULKAN_SDK}")
    set(Vulkan_DIR "${VULKAN_SDK}/lib/cmake/Vulkan")
    list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK}")
    set(Vulkan_LIBRARY "${VULKAN_SDK}/lib/VulkanLoader/lib/libvulkan.so")
    set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/include")
endif()

find_package(Vulkan REQUIRED)

# Fetch GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
if(APPLE)
    set(GLFW_BUILD_COCOA ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
else()
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.8
)
FetchContent_GetProperties(imgui)
if(NOT imgui_Populated)
    FetchContent_Populate(imgui)
endif()

# Define ImGui static library
set(IMGUI_SRC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)

# Locate glslc (shader compiler)
if(EXISTS "${VULKAN_SDK}/bin/glslc")
    set(GLSLC_EXECUTABLE "${VULKAN_SDK}/bin/glslc")
else()
    find_program(GLSLC_EXECUTABLE glslc REQUIRED)
endif()
message(STATUS "Using glslc at ${GLSLC_EXECUTABLE}")

# Shader compilation helper
function(compile_shader SHADER_FILE OUTPUT_FILE)
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER_FILE} -o ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling shader ${SHADER_FILE} to ${OUTPUT_FILE}"
    )
endfunction()

# Setup Shaders
set(SHADER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag
)
set(SHADER_SPV
    ${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv
    ${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv
)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert ${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv)
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag ${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv)

# Application Executable
add_executable(vulkan_app
    src/main.cpp
    src/vulkan_app.hpp
    src/vulkan_app.cpp
    ${SHADER_SPV}
)

target_include_directories(vulkan_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${glm_SOURCE_DIR}
)

target_link_libraries(vulkan_app PRIVATE
    Vulkan::Vulkan
    glfw
    imgui
)

if(APPLE)
    set_target_properties(vulkan_app PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "/opt/homebrew/lib")
endif()

# Copy shaders to build dir next to executable or define compile macros
target_compile_definitions(vulkan_app PRIVATE
    SHADER_DIR="${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)
