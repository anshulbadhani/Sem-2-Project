cmake_minimum_required(VERSION 3.16)
project(MatrixVisualizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer pkg-config (fallback when SFML_DIR fails)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML REQUIRED sfml-graphics sfml-window sfml-system)

# Include directories and linker flags from pkg-config
include_directories(${SFML_INCLUDE_DIRS})
link_directories(${SFML_LIBRARY_DIRS})

# Executable
add_executable(matrixSFML main.cpp)

# Link SFML libraries
target_link_libraries(matrixSFML ${SFML_LIBRARIES})

# Link macOS system frameworks (for SFML)
if(APPLE)
    target_link_libraries(matrixSFML
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework ApplicationServices"
        "-framework AudioToolbox"
        "-framework OpenAL"
    )
endif()