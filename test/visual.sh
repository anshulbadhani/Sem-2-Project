# #!/bin/bash

# # Exit on error
# set -e

# # --- Configuration ---
# EIGEN_PATH="/usr/include/eigen3"     # Adjust if needed
# BOOST_PATH="/usr/include"         # Adjust if needed
# SFML_INCLUDE_PATH="/usr/include"  # Adjust if needed (usually system path works)
# SFML_LIB_PATH="/usr/lib/x86_64-linux-gnu" # Adjust based on your distro/arch

# CXX="g++"
# CXX_STANDARD="-std=c++17"
# EXECUTABLE_NAME="simplex_plotter"

# # Source files for your CAS engine library (object files assumed pre-built or source included)
# CAS_ENGINE_SRC="linear_solver.cpp simplex_solver.cpp" # Or just simplex_solver.cpp if linear isn't needed
# MAIN_SRC="simplex_visualizer.cpp" # Name you save the visualization code as

# # Object files
# CAS_ENGINE_OBJ=$(echo "$CAS_ENGINE_SRC" | sed 's/\.cpp/\.o/g')
# MAIN_OBJ="${MAIN_SRC%.cpp}.o"
# ALL_OBJS="${CAS_ENGINE_OBJ} ${MAIN_OBJ}"

# # Include paths
# INCLUDE_PATHS="-I ${EIGEN_PATH} -I ${BOOST_PATH} -I ${SFML_INCLUDE_PATH}"

# # Linker flags for SFML (adjust based on your system)
# # Order matters for GCC/Clang linking! Graphics depends on Window, etc.
# SFML_LIBS="-lsfml-graphics -lsfml-window -lsfml-system"
# LDFLAGS="-L ${SFML_LIB_PATH} ${SFML_LIBS}" # Add Boost lib flags if necessary

# # --- Build Steps ---
# echo "Cleaning previous build..."
# rm -f ${ALL_OBJS} ${EXECUTABLE_NAME}

# echo "Compiling CAS engine sources..."
# for src in ${CAS_ENGINE_SRC}; do
#     obj="${src%.cpp}.o"
#     echo "  Compiling ${src} -> ${obj}"
#     $CXX ${CXX_STANDARD} ${INCLUDE_PATHS} -c ${src} -o ${obj}
# done

# echo "Compiling visualization source..."
# echo "  Compiling ${MAIN_SRC} -> ${MAIN_OBJ}"
# $CXX ${CXX_STANDARD} ${INCLUDE_PATHS} -c ${MAIN_SRC} -o ${MAIN_OBJ}


# echo "Linking..."
# $CXX ${CXX_STANDARD} ${ALL_OBJS} -o ${EXECUTABLE_NAME} ${LDFLAGS}

# echo "Build successful: ${EXECUTABLE_NAME}"
# echo "Ensure you have a font file (e.g., assets/fonts/DejaVuSans.ttf) accessible."
# echo "Run with: ./${EXECUTABLE_NAME}"

#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
# !!! IMPORTANT: Check these paths are correct for your system !!!
EIGEN_PATH="/usr/include/eigen3"     # Common path on Linux, adjust if needed
BOOST_PATH="/usr/include"         # Common path for Boost headers, adjust if needed
# SFML paths are NOT needed for this executable unless main.cpp uses it

CXX="g++" # Or clang++
CXX_STANDARD="-std=c++17" # C++17 is recommended
EXECUTABLE_NAME="cas_solver" # <<< CHANGED Executable Name

# Define ALL source files needed for the project libraries/components
# Add any other .cpp files your project uses here
ALL_COMPONENT_SRC="linear_solver.cpp simplex_solver.cpp lp_parser.cpp"

# Define the MAIN source file for THIS specific executable
MAIN_SRC="main.cpp" # <<< CHANGED Main Source File

# --- Automatic File Lists ---
# Create object file names from source file names
ALL_COMPONENT_OBJ=$(echo "$ALL_COMPONENT_SRC" | sed 's/\.cpp/\.o/g')
MAIN_OBJ="${MAIN_SRC%.cpp}.o"
# Combine all object files needed for linking this executable
ALL_OBJS="${ALL_COMPONENT_OBJ} ${MAIN_OBJ}"

# Include paths for headers (NO SFML NEEDED HERE)
INCLUDE_PATHS="-I ${EIGEN_PATH} -I ${BOOST_PATH} -I ." # Added -I . to search current dir

# Linker flags (libraries to link against)
# SFML libs are NOT needed here unless main.cpp uses them
SFML_LIBS=""
# Add Boost libs here IF needed (e.g., -lboost_program_options), not needed for headers-only parts
BOOST_LIBS=""
# Combine Linker Flags
LDFLAGS="${SFML_LIBS} ${BOOST_LIBS}" # <<< REMOVED SFML paths and libs

# --- Build Steps ---

# Clean previous build files for this target
echo "Cleaning previous build files (*.o matching sources, ${EXECUTABLE_NAME})..."
# Be more specific with rm to avoid deleting objects needed by other executables
rm -f ${MAIN_OBJ} ${EXECUTABLE_NAME}
# Optionally remove component objects too if you want a full clean build every time:
# rm -f ${ALL_OBJS} ${EXECUTABLE_NAME}


# Compile all component source files into object files (if not already up-to-date)
# This part is the same, it builds the shared library components
echo "Compiling component sources (if needed)..."
for src in ${ALL_COMPONENT_SRC}; do
    obj="${src%.cpp}.o"
    # Simple check: compile if object file doesn't exist or source is newer
    if [ ! -f "$obj" ] || [ "$src" -nt "$obj" ]; then
        # Check if source file exists before compiling
        if [ -f "$src" ]; then
            echo "  Compiling ${src} -> ${obj}"
            $CXX ${CXX_STANDARD} ${INCLUDE_PATHS} -c ${src} -o ${obj}
        else
            echo "  Warning: Component source file ${src} not found, skipping."
        fi
    else
        echo "  Skipping ${src} (object file ${obj} is up-to-date)."
    fi
done

# Compile the main source file for this specific executable
echo "Compiling main source (${MAIN_SRC})..."
# Check if main source file exists
if [ -f "$MAIN_SRC" ]; then
    # Compile if object file doesn't exist or source is newer
    if [ ! -f "$MAIN_OBJ" ] || [ "$MAIN_SRC" -nt "$MAIN_OBJ" ]; then
        echo "  Compiling ${MAIN_SRC} -> ${MAIN_OBJ}"
        $CXX ${CXX_STANDARD} ${INCLUDE_PATHS} -c ${MAIN_SRC} -o ${MAIN_OBJ}
    else
         echo "  Skipping ${MAIN_SRC} (object file ${MAIN_OBJ} is up-to-date)."
    fi
else
    echo "  Error: Main source file ${MAIN_SRC} not found! Cannot build ${EXECUTABLE_NAME}."
    exit 1
fi

# Link all necessary object files into the final executable
echo "Linking ${EXECUTABLE_NAME}..."
$CXX ${CXX_STANDARD} ${ALL_OBJS} -o ${EXECUTABLE_NAME} ${LDFLAGS}

echo ""
echo "Build successful: ${EXECUTABLE_NAME}"
echo "Run with: ./${EXECUTABLE_NAME}"

# --- Optional: Run Step ---
# Uncomment the following line to automatically run after building
# ./${EXECUTABLE_NAME}