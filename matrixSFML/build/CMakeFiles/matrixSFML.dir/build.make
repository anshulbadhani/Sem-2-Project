# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/anshul/Dev/projects/Sem-2-Project/matrixSFML

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build

# Include any dependencies generated for this target.
include CMakeFiles/matrixSFML.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/matrixSFML.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/matrixSFML.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/matrixSFML.dir/flags.make

CMakeFiles/matrixSFML.dir/main.cpp.o: CMakeFiles/matrixSFML.dir/flags.make
CMakeFiles/matrixSFML.dir/main.cpp.o: /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/main.cpp
CMakeFiles/matrixSFML.dir/main.cpp.o: CMakeFiles/matrixSFML.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/matrixSFML.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/matrixSFML.dir/main.cpp.o -MF CMakeFiles/matrixSFML.dir/main.cpp.o.d -o CMakeFiles/matrixSFML.dir/main.cpp.o -c /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/main.cpp

CMakeFiles/matrixSFML.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/matrixSFML.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/main.cpp > CMakeFiles/matrixSFML.dir/main.cpp.i

CMakeFiles/matrixSFML.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/matrixSFML.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/main.cpp -o CMakeFiles/matrixSFML.dir/main.cpp.s

# Object files for target matrixSFML
matrixSFML_OBJECTS = \
"CMakeFiles/matrixSFML.dir/main.cpp.o"

# External object files for target matrixSFML
matrixSFML_EXTERNAL_OBJECTS =

matrixSFML: CMakeFiles/matrixSFML.dir/main.cpp.o
matrixSFML: CMakeFiles/matrixSFML.dir/build.make
matrixSFML: CMakeFiles/matrixSFML.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable matrixSFML"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/matrixSFML.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/matrixSFML.dir/build: matrixSFML
.PHONY : CMakeFiles/matrixSFML.dir/build

CMakeFiles/matrixSFML.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/matrixSFML.dir/cmake_clean.cmake
.PHONY : CMakeFiles/matrixSFML.dir/clean

CMakeFiles/matrixSFML.dir/depend:
	cd /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/anshul/Dev/projects/Sem-2-Project/matrixSFML /home/anshul/Dev/projects/Sem-2-Project/matrixSFML /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build /home/anshul/Dev/projects/Sem-2-Project/matrixSFML/build/CMakeFiles/matrixSFML.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/matrixSFML.dir/depend

