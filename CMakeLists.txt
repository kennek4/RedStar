cmake_minimum_required(VERSION 3.10)
project(REDSTAR VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create an option to switch between a system sdl library and a vendored SDL
# library
option(REDSTAR_VENDORED "Use vendored libraries" OFF)

if(REDSTAR_VENDORED)
  # This assumes you have added SDL as a submodule in vendored/SDL
  add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
  # 1. Look for a SDL3 package,
  # 2. look for the SDL3-shared component, and
  # 3. fail if the shared component cannot be found.
  find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
  find_package(SDL3_image REQUIRED) # Find the SDL3_image package
endif()

# Get OpenGL Packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Directories
set(SCRIPT_DIR "scripts/")
set(HEADER_DIR "scripts/headers/")
set(CLASSES_DIR "scripts/classes/")

# Source files
set(SCRIPT_SOURCES ${SCRIPT_DIR}main.cpp)
set(HEADER_SOURCES ${HEADER_DIR})
set(CLASS_SOURCES ${CLASSES_DIR})

# Create your game executable target as usual
add_executable(REDSTAR ${HEADER_SOURCES} ${CLASS_SOURCES} ${SCRIPT_SOURCES})

target_include_directories(REDSTAR PRIVATE scripts)

# Link to the actual SDL3 library.
target_link_libraries(REDSTAR PRIVATE SDL3_image::SDL3_image SDL3::SDL3
                                      OpenGL::OpenGL ${GLFW3_LIBRARY})
