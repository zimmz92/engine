# CMakeList.txt : CMake project for Arundos, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.25.2)
project (Arundos VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "64-bit architecture detected.")
    add_compile_definitions( ARCH_64BIT )
else ()
    message(STATUS "32-bit architecture detected.")
    add_compile_definitions( ARCH_32BIT )
endif ()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_compile_definitions( MY_DEBUG )
    #add_compile_definitions( ECS_DEBUG )
    #add_compile_definitions( FPS_DEBUG )
else()
    #add_compile_definitions( ECS_DEBUG )
endif()


# Add Vulkan and dependant libraries to the project
find_package(Vulkan REQUIRED)

# Add glfw library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Compile Shaders
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/engine/graphics/shaders)
set(SHADER_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/engines/graphics/shaders)
file(GLOB SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag ${SHADER_DIR}/*.comp ${SHADER_DIR}/*.geom ${SHADER_DIR}/*.tesc ${SHADER_DIR}/*.tese ${SHADER_DIR}/*.mesh ${SHADER_DIR}/*.task ${SHADER_DIR}/*.rgen ${SHADER_DIR}/*.rchit ${SHADER_DIR}/*.rmiss)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(OUTPUT ${SHADER_BUILD_DIR}/${FILENAME}.spv
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER_BUILD_DIR}/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling ${FILENAME}")
    list(APPEND SPV_SHADERS ${SHADER_BUILD_DIR}/${FILENAME}.spv)
endForeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

add_executable (Arundos main.cpp)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Arundos PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )


add_subdirectory(Libraries/glfw3)
add_subdirectory(Libraries/glm)

# Add source to this project's executable.
add_library(mySrcFiles "")

add_subdirectory(engine)

add_subdirectory(engine/resource_manager)

add_subdirectory(engine/memory)

add_subdirectory(engine/graphics)
add_subdirectory(engine/graphics/systems)
add_subdirectory(engine/graphics/systems/render_systems)

add_subdirectory(engine/ecs)
add_subdirectory(engine/ecs/test_code)

add_subdirectory(engine/library)

add_subdirectory(game_code)
add_subdirectory(game_code/entities)
add_subdirectory(game_code/components)
add_subdirectory(game_code/systems)

add_subdirectory(Libraries/tinyobjectloader)
add_subdirectory(Libraries/easyloggingpp)
add_subdirectory(Libraries/stb)

# Add asset folders
add_subdirectory(assets/audio)
add_subdirectory(assets/materials)
add_subdirectory(assets/models)
add_subdirectory(assets/ui_textures)

# Add NON-PRODUCTION asset folders
add_subdirectory(assets_NOTPRODUCTION/audio)
add_subdirectory(assets_NOTPRODUCTION/materials)
add_subdirectory(assets_NOTPRODUCTION/models)
add_subdirectory(assets_NOTPRODUCTION/ui_textures)
add_subdirectory(test_code_NOTPRODUCTION)

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

ADD_DEPENDENCIES(mySrcFiles shaders)
ADD_DEPENDENCIES(Arundos shaders)