# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /opt/clion-2021.1.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2021.1.1/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /user/7/volandd/Documents/2A/SEPC/tp1alloc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/memshell.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/memshell.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/memshell.dir/flags.make

CMakeFiles/memshell.dir/src/memshell.c.o: CMakeFiles/memshell.dir/flags.make
CMakeFiles/memshell.dir/src/memshell.c.o: ../src/memshell.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/memshell.dir/src/memshell.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/memshell.dir/src/memshell.c.o -c /user/7/volandd/Documents/2A/SEPC/tp1alloc/src/memshell.c

CMakeFiles/memshell.dir/src/memshell.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/memshell.dir/src/memshell.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /user/7/volandd/Documents/2A/SEPC/tp1alloc/src/memshell.c > CMakeFiles/memshell.dir/src/memshell.c.i

CMakeFiles/memshell.dir/src/memshell.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/memshell.dir/src/memshell.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /user/7/volandd/Documents/2A/SEPC/tp1alloc/src/memshell.c -o CMakeFiles/memshell.dir/src/memshell.c.s

# Object files for target memshell
memshell_OBJECTS = \
"CMakeFiles/memshell.dir/src/memshell.c.o"

# External object files for target memshell
memshell_EXTERNAL_OBJECTS =

memshell: CMakeFiles/memshell.dir/src/memshell.c.o
memshell: CMakeFiles/memshell.dir/build.make
memshell: libemalloc.so
memshell: CMakeFiles/memshell.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable memshell"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/memshell.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/memshell.dir/build: memshell

.PHONY : CMakeFiles/memshell.dir/build

CMakeFiles/memshell.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/memshell.dir/cmake_clean.cmake
.PHONY : CMakeFiles/memshell.dir/clean

CMakeFiles/memshell.dir/depend:
	cd /user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /user/7/volandd/Documents/2A/SEPC/tp1alloc /user/7/volandd/Documents/2A/SEPC/tp1alloc /user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug /user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug /user/7/volandd/Documents/2A/SEPC/tp1alloc/cmake-build-debug/CMakeFiles/memshell.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/memshell.dir/depend
