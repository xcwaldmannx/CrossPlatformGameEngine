# CMakeList.txt : CMake project for CrossPlatformGameEngine, include source and define
# project specific logic here.
#

include(FetchContent)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")

# Add source to this project's executable.
add_executable (CrossPlatformGameEngine ${SRC_FILES} "src/main/Components.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CrossPlatformGameEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# VulkanSDK

find_package(Vulkan REQUIRED)

# link Vulkan
target_link_libraries(CrossPlatformGameEngine PRIVATE Vulkan::Vulkan)
target_link_directories(CrossPlatformGameEngine PRIVATE Vulkan_INCLUDE_DIRS)

message(STATUS "Vulkan Lib Path: ${Vulkan_LIBRARIES}")
message(STATUS "Vulkan Include Path: ${Vulkan_INCLUDE_DIRS}")

# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable GLFW documentation" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Disable GLFW tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable GLFW examples" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build GLFW as a static library" FORCE)

# Add the GLFW source directory
add_subdirectory(${CMAKE_SOURCE_DIR}/Vendor/glfw3 ${CMAKE_BINARY_DIR}/glfw3_build)

# Link GLFW and OpenGL
target_link_libraries(CrossPlatformGameEngine PRIVATE glfw opengl32)

# include stb_image
target_include_directories(CrossPlatformGameEngine PRIVATE "${CMAKE_SOURCE_DIR}/Vendor/stb_image/include")

# Link glm

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

target_link_libraries(CrossPlatformGameEngine PRIVATE glm::glm)


# link assimp

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.4.3
)

FetchContent_MakeAvailable(assimp)

target_link_libraries(CrossPlatformGameEngine PRIVATE assimp)

# copy shaders to build dir

configure_file(
    ${CMAKE_SOURCE_DIR}/CrossPlatformGameEngine/src/shaders/vert.spv # Source file
    ${CMAKE_BINARY_DIR}/CrossPlatformGameEngine/src/shaders/vert.spv # Destination
    COPYONLY                               # Prevents file processing
)

configure_file(
    ${CMAKE_SOURCE_DIR}/CrossPlatformGameEngine/src/shaders/frag.spv # Source file
    ${CMAKE_BINARY_DIR}/CrossPlatformGameEngine/src/shaders/frag.spv # Destination
    COPYONLY                               # Prevents file processing
)
