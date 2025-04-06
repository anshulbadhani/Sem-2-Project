# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(OpenGLTransformations)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Find OpenGL ---
find_package(OpenGL REQUIRED)

# --- Find GLEW ---
# Install: sudo apt-get install libglew-dev (Debian/Ubuntu)
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found!")
endif()

# --- Find GLFW ---
# Install: sudo apt-get install libglfw3-dev (Debian/Ubuntu)
find_package(glfw3 REQUIRED)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW3 not found!")
endif()

# --- Find GLM ---
# Install: sudo apt-get install libglm-dev (Debian/Ubuntu)
# Or download to e.g. 'external/glm' and uncomment the target_include_directories line below
find_package(glm REQUIRED)
if(NOT glm_FOUND)
    message(FATAL_ERROR "GLM not found!")
endif()

# --- Define the Executable ---
add_executable(transform_vis
    src/main.cpp
    src/shader.cpp
    src/renderer.cpp
    # Add any other .cpp files from src/ here
)

# --- Specify Include Directories for the Target ---
# This tells CMake (and IDEs using CMake Tools) where to find your project's headers
target_include_directories(transform_vis
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include # Make the 'include' directory available
    # PRIVATE # Only list external includes here if linking doesn't automatically add them
        # ${OPENGL_INCLUDE_DIR} # Usually handled by linking OpenGL::GL
        # ${GLEW_INCLUDE_DIRS}    # Usually handled by linking GLEW::GLEW
        # ${glfw3_INCLUDE_DIRS}   # Usually handled by linking glfw
        # ${CMAKE_CURRENT_SOURCE_DIR}/external/glm # Uncomment if GLM is local and not found by find_package
)

# --- Link Libraries to the Target ---
# Modern CMake targets (like OpenGL::GL, GLEW::GLEW, glfw, glm::glm)
# often automatically manage their include directories when linked.
target_link_libraries(transform_vis
    PRIVATE
        OpenGL::GL   # Modern target for OpenGL
        GLEW::GLEW   # Modern target for GLEW
        glfw         # Target name for GLFW (might vary slightly based on FindGLFW)
        glm::glm     # Modern target for GLM
)

# --- Copy Shaders ---
# Copies the 'shaders' directory to the same directory as the executable after build
add_custom_command(TARGET transform_vis POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:transform_vis>/shaders
    COMMENT "Copying shaders"
)

# --- Optional: Set Properties for IDEs (like VS Code with CMake Tools) ---
# Helps IDEs find source files if they are not directly under the root
set_target_properties(transform_vis PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)
# For VS Code, make sure CMAKE_EXPORT_COMPILE_COMMANDS is ON
# (often a default setting in CMake Tools or can be set via CMakePresets.json or command line -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
# This helps IntelliSense find files. You might not need this explicitly.
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)