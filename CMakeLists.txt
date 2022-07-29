# CMakeList.txt : CMake project for Arundos, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.15)
project (Arundos VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specific File Paths
file (TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/engines/graphics/shaders/compile.bat" BATFILE)
file (TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/engines/graphics/shaders" SHADERPATH)

if(WIN32)
    file (TO_NATIVE_PATH "C:/VulkanSDK/1.2.182.0/Bin32" VULKANDIR)
endif()

add_executable (Arundos main.cpp)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Arundos PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )

# Add Vulkan and dependant libraries to the project
find_package(Vulkan REQUIRED)

# Add glfw library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Libraries/glfw3)
add_subdirectory(Libraries/glm)

# Add source to this project's executable.
add_library(mySrcFiles "")

add_subdirectory(engines/graphics)
add_subdirectory(engines/graphics/shaders)

add_subdirectory(engines/ecs)

add_subdirectory(game_code)
add_subdirectory(game_code/objects)
add_subdirectory(game_code/render_systems)

add_subdirectory(Libraries/tinyobjectloader)

add_subdirectory(models)

target_sources(mySrcFiles
    PRIVATE
        Arundos.cpp
    PUBLIC
)
target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(mySrcFiles PUBLIC Vulkan::Vulkan glfw glm)

target_link_libraries(${PROJECT_NAME} PUBLIC ${EXTRA_LIBS} PRIVATE mySrcFiles Vulkan::Vulkan glfw glm)

# Configure files
configure_file(Arundos.hpp.in Arundos.hpp)

# Include the binary files so the configured .hpp can be found
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_custom_target(GenerateShaders ALL
                  COMMAND CMD /D /C "${BATFILE}" "${CMAKE_CURRENT_SOURCE_DIR}" "${VULKANDIR}"
                  DEPENDS ${SHADERPATH})

ADD_DEPENDENCIES(mySrcFiles GenerateShaders)
ADD_DEPENDENCIES(Arundos GenerateShaders)