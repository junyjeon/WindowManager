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
CMAKE_SOURCE_DIR = /home/jun/11_November/WindowManager

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jun/11_November/WindowManager/build

# Include any dependencies generated for this target.
include CMakeFiles/WindowManager.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/WindowManager.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/WindowManager.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WindowManager.dir/flags.make

CMakeFiles/WindowManager.dir/src/main.cpp.o: CMakeFiles/WindowManager.dir/flags.make
CMakeFiles/WindowManager.dir/src/main.cpp.o: /home/jun/11_November/WindowManager/src/main.cpp
CMakeFiles/WindowManager.dir/src/main.cpp.o: CMakeFiles/WindowManager.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/jun/11_November/WindowManager/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/WindowManager.dir/src/main.cpp.o"
	x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WindowManager.dir/src/main.cpp.o -MF CMakeFiles/WindowManager.dir/src/main.cpp.o.d -o CMakeFiles/WindowManager.dir/src/main.cpp.o -c /home/jun/11_November/WindowManager/src/main.cpp

CMakeFiles/WindowManager.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/WindowManager.dir/src/main.cpp.i"
	x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jun/11_November/WindowManager/src/main.cpp > CMakeFiles/WindowManager.dir/src/main.cpp.i

CMakeFiles/WindowManager.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/WindowManager.dir/src/main.cpp.s"
	x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jun/11_November/WindowManager/src/main.cpp -o CMakeFiles/WindowManager.dir/src/main.cpp.s

CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o: CMakeFiles/WindowManager.dir/flags.make
CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o: /home/jun/11_November/WindowManager/src/WindowManager.cpp
CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o: CMakeFiles/WindowManager.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/jun/11_November/WindowManager/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o"
	x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o -MF CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o.d -o CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o -c /home/jun/11_November/WindowManager/src/WindowManager.cpp

CMakeFiles/WindowManager.dir/src/WindowManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/WindowManager.dir/src/WindowManager.cpp.i"
	x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jun/11_November/WindowManager/src/WindowManager.cpp > CMakeFiles/WindowManager.dir/src/WindowManager.cpp.i

CMakeFiles/WindowManager.dir/src/WindowManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/WindowManager.dir/src/WindowManager.cpp.s"
	x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jun/11_November/WindowManager/src/WindowManager.cpp -o CMakeFiles/WindowManager.dir/src/WindowManager.cpp.s

# Object files for target WindowManager
WindowManager_OBJECTS = \
"CMakeFiles/WindowManager.dir/src/main.cpp.o" \
"CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o"

# External object files for target WindowManager
WindowManager_EXTERNAL_OBJECTS =

WindowManager: CMakeFiles/WindowManager.dir/src/main.cpp.o
WindowManager: CMakeFiles/WindowManager.dir/src/WindowManager.cpp.o
WindowManager: CMakeFiles/WindowManager.dir/build.make
WindowManager: CMakeFiles/WindowManager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/jun/11_November/WindowManager/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable WindowManager"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WindowManager.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WindowManager.dir/build: WindowManager
.PHONY : CMakeFiles/WindowManager.dir/build

CMakeFiles/WindowManager.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WindowManager.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WindowManager.dir/clean

CMakeFiles/WindowManager.dir/depend:
	cd /home/jun/11_November/WindowManager/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jun/11_November/WindowManager /home/jun/11_November/WindowManager /home/jun/11_November/WindowManager/build /home/jun/11_November/WindowManager/build /home/jun/11_November/WindowManager/build/CMakeFiles/WindowManager.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/WindowManager.dir/depend

