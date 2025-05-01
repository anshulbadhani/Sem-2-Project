#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
# !!! IMPORTANT: Set these paths correctly for your system !!!
EIGEN_PATH="/usr/include/eigen3"  # Common path on Linux, adjust if needed
BOOST_PATH="/usr/include"          # Common path for Boost headers, adjust if needed
# If Boost headers are in a subdirectory like /usr/include/boost, the -I /usr/include should still work

CXX="g++" # Or clang++
CXX_STANDARD="-std=c++17" # C++17 is recommended for Eigen/modern C++ features
EXECUTABLE_NAME="cas_main"
SOURCE_FILES="linear_solver.cpp simplex_solver.cpp main.cpp"
OBJECT_FILES="linear_solver.o simplex_solver.o main.o"
INCLUDE_PATHS="-I ${EIGEN_PATH} -I ${BOOST_PATH}"
# Add Boost library path if needed for compiled Boost libs (not needed for rational/lexical_cast)
# LDFLAGS="-L /path/to/boost/libs -lboost_system ..."

# --- Build Steps ---

# Clean previous build (optional but good practice)
echo "Cleaning previous build files..."
rm -f ${OBJECT_FILES} ${EXECUTABLE_NAME}

# Compile object files
echo "Compiling source files..."
for src_file in ${SOURCE_FILES}; do
    obj_file="${src_file%.cpp}.o"
    echo "  Compiling ${src_file} -> ${obj_file}"
    $CXX ${CXX_STANDARD} ${INCLUDE_PATHS} -c ${src_file} -o ${obj_file}
done

# Link object files into an executable
echo "Linking object files into ${EXECUTABLE_NAME}..."
$CXX ${CXX_STANDARD} ${OBJECT_FILES} -o ${EXECUTABLE_NAME} # ${LDFLAGS} # Add LDFLAGS if needed

echo "Build successful: ${EXECUTABLE_NAME}"

# --- Run Step (Optional) ---
# Uncomment the following lines to automatically run after building
# echo "Running ${EXECUTABLE_NAME}..."
# ./${EXECUTABLE_NAME}
# echo "Execution finished."

echo "Done."