# CMakeList.txt : CMake project for Arundos, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (Arundos VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(Arundos.h.in Arundos.h)

# Add source to this project's executable.
add_executable (Arundos Arundos.cpp)

#target_sources(Arundos PRIVATE Arundos.cpp)

# Add Vulkan and dependant libraries to the project
#find_package(Vulkan REQUIRED)
#target_link_libraries(Arundos PRIVATE Vulkan::Vulkan)

#add_subdirectory(glfw)
#target_link_libraries(Arundos PUBLIC glfw)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Arundos PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          )



# TODO: Add tests and install targets if needed.
