# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /home/mariomain/executables/cmake-3.22.0-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /home/mariomain/executables/cmake-3.22.0-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out

# Include any dependencies generated for this target.
include uncrc/CMakeFiles/uncrc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include uncrc/CMakeFiles/uncrc.dir/compiler_depend.make

# Include the progress variables for this target.
include uncrc/CMakeFiles/uncrc.dir/progress.make

# Include the compile flags for this target's objects.
include uncrc/CMakeFiles/uncrc.dir/flags.make

uncrc/CMakeFiles/uncrc.dir/main.cpp.o: uncrc/CMakeFiles/uncrc.dir/flags.make
uncrc/CMakeFiles/uncrc.dir/main.cpp.o: ../uncrc/main.cpp
uncrc/CMakeFiles/uncrc.dir/main.cpp.o: uncrc/CMakeFiles/uncrc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object uncrc/CMakeFiles/uncrc.dir/main.cpp.o"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT uncrc/CMakeFiles/uncrc.dir/main.cpp.o -MF CMakeFiles/uncrc.dir/main.cpp.o.d -o CMakeFiles/uncrc.dir/main.cpp.o -c /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/uncrc/main.cpp

uncrc/CMakeFiles/uncrc.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/uncrc.dir/main.cpp.i"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/uncrc/main.cpp > CMakeFiles/uncrc.dir/main.cpp.i

uncrc/CMakeFiles/uncrc.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/uncrc.dir/main.cpp.s"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/uncrc/main.cpp -o CMakeFiles/uncrc.dir/main.cpp.s

# Object files for target uncrc
uncrc_OBJECTS = \
"CMakeFiles/uncrc.dir/main.cpp.o"

# External object files for target uncrc
uncrc_EXTERNAL_OBJECTS =

uncrc/uncrc: uncrc/CMakeFiles/uncrc.dir/main.cpp.o
uncrc/uncrc: uncrc/CMakeFiles/uncrc.dir/build.make
uncrc/uncrc: uncrc/CMakeFiles/uncrc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable uncrc"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/uncrc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
uncrc/CMakeFiles/uncrc.dir/build: uncrc/uncrc
.PHONY : uncrc/CMakeFiles/uncrc.dir/build

uncrc/CMakeFiles/uncrc.dir/clean:
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc && $(CMAKE_COMMAND) -P CMakeFiles/uncrc.dir/cmake_clean.cmake
.PHONY : uncrc/CMakeFiles/uncrc.dir/clean

uncrc/CMakeFiles/uncrc.dir/depend:
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/uncrc /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/uncrc/CMakeFiles/uncrc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : uncrc/CMakeFiles/uncrc.dir/depend

