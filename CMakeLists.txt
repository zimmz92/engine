# CMakeList.txt : CMake project for Arundos, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (Arundos VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(Arundos.h.in Arundos.h)

# Add source to this project's executable.
add_executable (Arundos Arundos.cpp)

# Add Vulkan and dependant libraries to the project
find_package(Vulkan REQUIRED)

# Add additional libraries for Vulkan
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Libraries/glfw3)

add_subdirectory(Libraries/glm)

include_directories(engines)
include_directories(engines/graphics)
include_directories(engines/graphics/shaders)

target_link_libraries(Arundos PUBLIC ${EXTRA_LIBS} PRIVATE Vulkan::Vulkan glfw glm)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Arundos PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )

# Compile shaders for post build reference
#if(WIN32)
#  add_custom_command(TARGET <Your target>
#                       POST_BUILD
#                       COMMAND cmd /c C:\Users\halo9\Source\Repos\arundos\Engines\Graphics\shaders\compile.bat )
#
#endif()

# Add required files required for post build reference
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/engines/graphics/shaders/vert.spv ${CMAKE_CURRENT_BINARY_DIR}/engines/graphics/shaders/vert.spv COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/engines/graphics/shaders/frag.spv ${CMAKE_CURRENT_BINARY_DIR}/engines/graphics/shaders/frag.spv COPYONLY)


# TODO: Add tests and install targets if needed.
