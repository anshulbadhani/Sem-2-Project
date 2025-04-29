# In your CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Eigen3 installed via apt
# The system installation provides the necessary FindEigen3.cmake files
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)

add_executable(solver src/main.cpp)

# Link Eigen headers to your target
# Option A (Modern CMake - Recommended): Link the imported target
target_link_libraries(solver PRIVATE Eigen3::Eigen)

# Option B (Older CMake - Also works): Include directories directly
# target_include_directories(my_app PRIVATE ${EIGEN3_INCLUDE_DIRS})