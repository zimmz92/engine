# CMakeList.txt : CMake project for Arundos, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.15)
project (Arundos VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define standard directories
#set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Libraries)

# Configure files
configure_file(Arundos.hpp.in Arundos.hpp)

# Include the binary files so the configured .hpp can be found
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add source to this project's executable.
add_library(mySrcFiles "")
add_subdirectory(engines/graphics)
add_subdirectory(engines/graphics/shaders)

target_sources(mySrcFiles
    PRIVATE
        Arundos.cpp
    PUBLIC
)
target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(mySrcFiles PUBLIC Vulkan::Vulkan glfw glm)

add_executable (Arundos main.cpp)

# Add Vulkan and dependant libraries to the project
find_package(Vulkan REQUIRED)

# Add glfw library
#set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
#set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
#set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
#set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Libraries/glfw3)
add_subdirectory(Libraries/glm)


target_link_libraries(${PROJECT_NAME} PUBLIC ${EXTRA_LIBS} PRIVATE mySrcFiles Vulkan::Vulkan glfw glm)

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


# TODO: Add tests and install targets if needed.
