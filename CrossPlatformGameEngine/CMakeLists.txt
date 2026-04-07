cmake_minimum_required(VERSION 3.12)
project(CrossPlatformGameEngine LANGUAGES CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")
list(FILTER SRC_FILES EXCLUDE REGEX "BadGraphicsPipeline")

add_executable(CrossPlatformGameEngine ${SRC_FILES})

set_property(TARGET CrossPlatformGameEngine PROPERTY CXX_STANDARD 23)

# build everything static by default
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# -------------------------
# Vulkan
# -------------------------
find_package(Vulkan REQUIRED)

target_link_libraries(CrossPlatformGameEngine PRIVATE Vulkan::Vulkan)
target_include_directories(CrossPlatformGameEngine PRIVATE ${Vulkan_INCLUDE_DIRS})

message(STATUS "Vulkan Version: ${Vulkan_VERSION}")
message(STATUS "Vulkan Lib Path: ${Vulkan_LIBRARIES}")
message(STATUS "Vulkan Include Path: ${Vulkan_INCLUDE_DIRS}")

# -------------------------
# GLFW
# -------------------------
if (UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GLFW REQUIRED glfw3)

	target_include_directories(CrossPlatformGameEngine PRIVATE ${GLFW_INCLUDE_DIRS})
	target_link_directories(CrossPlatformGameEngine PRIVATE ${GLFW_LIBRARY_DIRS})
	target_link_libraries(CrossPlatformGameEngine PRIVATE ${GLFW_LIBRARIES})
elseif(WIN32)
	find_package(glfw3 CONFIG REQUIRED)
	target_link_libraries(CrossPlatformGameEngine PRIVATE glfw)
endif()

# -------------------------
# OpenGL
# -------------------------
find_package(OpenGL REQUIRED)
target_link_libraries(CrossPlatformGameEngine PRIVATE OpenGL::GL)

# -------------------------
# stb_image include
# -------------------------
target_include_directories(CrossPlatformGameEngine PRIVATE "${CMAKE_SOURCE_DIR}/Vendor/stb")

# -------------------------
# glm
# -------------------------

find_package(glm REQUIRED)
target_link_libraries(CrossPlatformGameEngine PUBLIC glm::glm)

# -------------------------
# assimp
# -------------------------

find_package(assimp REQUIRED)

target_link_libraries(CrossPlatformGameEngine PRIVATE
		assimp::assimp
)

# -------------------------
# mass (prebuilt static .a)
# -------------------------
target_include_directories(CrossPlatformGameEngine PRIVATE "${CMAKE_SOURCE_DIR}/Vendor/mass/include")

target_link_libraries(CrossPlatformGameEngine PRIVATE
		debug     "${CMAKE_SOURCE_DIR}/Vendor/mass/lib/libModelAttributeSelectiveSerializerd.a"
		optimized "${CMAKE_SOURCE_DIR}/Vendor/mass/lib/libModelAttributeSelectiveSerializer.a"
)

# -------------------------
# copy shaders/assets
# -------------------------

set(ENGINE_SHADERS_DIR "${CMAKE_SOURCE_DIR}/CrossPlatformGameEngine/src/shaders")

if(UNIX AND NOT APPLE)
	set(ENGINE_ASSETS_SOURCE_DIR "/home/cwaldmann/Documents/Workspaces/GameEngine/assets")
elseif(WIN32)
	set(ENGINE_ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets") # adjust
endif()

add_custom_command(TARGET CrossPlatformGameEngine POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:CrossPlatformGameEngine>/src/shaders"
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${ENGINE_SHADERS_DIR}"
		"$<TARGET_FILE_DIR:CrossPlatformGameEngine>/src/shaders"

		COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:CrossPlatformGameEngine>/assets"
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${ENGINE_ASSETS_SOURCE_DIR}"
		"$<TARGET_FILE_DIR:CrossPlatformGameEngine>/assets"

		COMMENT "Copying shaders and other assets to executable location..."
)
