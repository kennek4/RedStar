# Add the directory containing FindGLFW3.cmake to the module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") # Or appropriate path

# Find GLFW3 (request a minimum version, make it required)
find_package(GLFW3 3.3 REQUIRED)

# ... define your executable target ...
add_executable(my_app main.cpp)

# Link against the imported target (Recommended)
if(TARGET GLFW::glfw)
    target_link_libraries(my_app PRIVATE GLFW::glfw)
    message(STATUS "Linking against imported target GLFW::glfw")
else()
    # Fallback for very old projects (Not Recommended)
    target_include_directories(my_app PRIVATE ${GLFW3_INCLUDE_DIRS})
    target_link_libraries(my_app PRIVATE ${GLFW3_LIBRARIES})
    message(WARNING "Linking against legacy GLFW3 variables. Consider using the GLFW::glfw target.")
endif()
