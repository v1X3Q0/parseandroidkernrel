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
include c_reginst/CMakeFiles/c_reginst.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include c_reginst/CMakeFiles/c_reginst.dir/compiler_depend.make

# Include the progress variables for this target.
include c_reginst/CMakeFiles/c_reginst.dir/progress.make

# Include the compile flags for this target's objects.
include c_reginst/CMakeFiles/c_reginst.dir/flags.make

c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.o: c_reginst/CMakeFiles/c_reginst.dir/flags.make
c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.o: /home/mariomain/repos/postec_plural/common/c_reginst/opcOperand.cpp
c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.o: c_reginst/CMakeFiles/c_reginst.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.o"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.o -MF CMakeFiles/c_reginst.dir/opcOperand.cpp.o.d -o CMakeFiles/c_reginst.dir/opcOperand.cpp.o -c /home/mariomain/repos/postec_plural/common/c_reginst/opcOperand.cpp

c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/c_reginst.dir/opcOperand.cpp.i"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mariomain/repos/postec_plural/common/c_reginst/opcOperand.cpp > CMakeFiles/c_reginst.dir/opcOperand.cpp.i

c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/c_reginst.dir/opcOperand.cpp.s"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mariomain/repos/postec_plural/common/c_reginst/opcOperand.cpp -o CMakeFiles/c_reginst.dir/opcOperand.cpp.s

c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.o: c_reginst/CMakeFiles/c_reginst.dir/flags.make
c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.o: /home/mariomain/repos/postec_plural/common/c_reginst/ibeSet.cpp
c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.o: c_reginst/CMakeFiles/c_reginst.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.o"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.o -MF CMakeFiles/c_reginst.dir/ibeSet.cpp.o.d -o CMakeFiles/c_reginst.dir/ibeSet.cpp.o -c /home/mariomain/repos/postec_plural/common/c_reginst/ibeSet.cpp

c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/c_reginst.dir/ibeSet.cpp.i"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mariomain/repos/postec_plural/common/c_reginst/ibeSet.cpp > CMakeFiles/c_reginst.dir/ibeSet.cpp.i

c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/c_reginst.dir/ibeSet.cpp.s"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mariomain/repos/postec_plural/common/c_reginst/ibeSet.cpp -o CMakeFiles/c_reginst.dir/ibeSet.cpp.s

# Object files for target c_reginst
c_reginst_OBJECTS = \
"CMakeFiles/c_reginst.dir/opcOperand.cpp.o" \
"CMakeFiles/c_reginst.dir/ibeSet.cpp.o"

# External object files for target c_reginst
c_reginst_EXTERNAL_OBJECTS =

c_reginst/libc_reginst.a: c_reginst/CMakeFiles/c_reginst.dir/opcOperand.cpp.o
c_reginst/libc_reginst.a: c_reginst/CMakeFiles/c_reginst.dir/ibeSet.cpp.o
c_reginst/libc_reginst.a: c_reginst/CMakeFiles/c_reginst.dir/build.make
c_reginst/libc_reginst.a: c_reginst/CMakeFiles/c_reginst.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libc_reginst.a"
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && $(CMAKE_COMMAND) -P CMakeFiles/c_reginst.dir/cmake_clean_target.cmake
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/c_reginst.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
c_reginst/CMakeFiles/c_reginst.dir/build: c_reginst/libc_reginst.a
.PHONY : c_reginst/CMakeFiles/c_reginst.dir/build

c_reginst/CMakeFiles/c_reginst.dir/clean:
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst && $(CMAKE_COMMAND) -P CMakeFiles/c_reginst.dir/cmake_clean.cmake
.PHONY : c_reginst/CMakeFiles/c_reginst.dir/clean

c_reginst/CMakeFiles/c_reginst.dir/depend:
	cd /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel /home/mariomain/repos/postec_plural/common/c_reginst /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst /home/mariomain/repos/postec_plural/linux/helpers/tools/parseandroidkernrel/build-out/c_reginst/CMakeFiles/c_reginst.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : c_reginst/CMakeFiles/c_reginst.dir/depend
