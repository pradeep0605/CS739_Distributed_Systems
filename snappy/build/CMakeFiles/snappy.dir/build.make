# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /u/s/h/sharath/private/cs739/snappy

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /u/s/h/sharath/private/cs739/snappy/build

# Include any dependencies generated for this target.
include CMakeFiles/snappy.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/snappy.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/snappy.dir/flags.make

CMakeFiles/snappy.dir/snappy-c.cc.o: CMakeFiles/snappy.dir/flags.make
CMakeFiles/snappy.dir/snappy-c.cc.o: ../snappy-c.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/u/s/h/sharath/private/cs739/snappy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/snappy.dir/snappy-c.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/snappy.dir/snappy-c.cc.o -c /u/s/h/sharath/private/cs739/snappy/snappy-c.cc

CMakeFiles/snappy.dir/snappy-c.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/snappy.dir/snappy-c.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /u/s/h/sharath/private/cs739/snappy/snappy-c.cc > CMakeFiles/snappy.dir/snappy-c.cc.i

CMakeFiles/snappy.dir/snappy-c.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/snappy.dir/snappy-c.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /u/s/h/sharath/private/cs739/snappy/snappy-c.cc -o CMakeFiles/snappy.dir/snappy-c.cc.s

CMakeFiles/snappy.dir/snappy-c.cc.o.requires:

.PHONY : CMakeFiles/snappy.dir/snappy-c.cc.o.requires

CMakeFiles/snappy.dir/snappy-c.cc.o.provides: CMakeFiles/snappy.dir/snappy-c.cc.o.requires
	$(MAKE) -f CMakeFiles/snappy.dir/build.make CMakeFiles/snappy.dir/snappy-c.cc.o.provides.build
.PHONY : CMakeFiles/snappy.dir/snappy-c.cc.o.provides

CMakeFiles/snappy.dir/snappy-c.cc.o.provides.build: CMakeFiles/snappy.dir/snappy-c.cc.o


CMakeFiles/snappy.dir/snappy-sinksource.cc.o: CMakeFiles/snappy.dir/flags.make
CMakeFiles/snappy.dir/snappy-sinksource.cc.o: ../snappy-sinksource.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/u/s/h/sharath/private/cs739/snappy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/snappy.dir/snappy-sinksource.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/snappy.dir/snappy-sinksource.cc.o -c /u/s/h/sharath/private/cs739/snappy/snappy-sinksource.cc

CMakeFiles/snappy.dir/snappy-sinksource.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/snappy.dir/snappy-sinksource.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /u/s/h/sharath/private/cs739/snappy/snappy-sinksource.cc > CMakeFiles/snappy.dir/snappy-sinksource.cc.i

CMakeFiles/snappy.dir/snappy-sinksource.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/snappy.dir/snappy-sinksource.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /u/s/h/sharath/private/cs739/snappy/snappy-sinksource.cc -o CMakeFiles/snappy.dir/snappy-sinksource.cc.s

CMakeFiles/snappy.dir/snappy-sinksource.cc.o.requires:

.PHONY : CMakeFiles/snappy.dir/snappy-sinksource.cc.o.requires

CMakeFiles/snappy.dir/snappy-sinksource.cc.o.provides: CMakeFiles/snappy.dir/snappy-sinksource.cc.o.requires
	$(MAKE) -f CMakeFiles/snappy.dir/build.make CMakeFiles/snappy.dir/snappy-sinksource.cc.o.provides.build
.PHONY : CMakeFiles/snappy.dir/snappy-sinksource.cc.o.provides

CMakeFiles/snappy.dir/snappy-sinksource.cc.o.provides.build: CMakeFiles/snappy.dir/snappy-sinksource.cc.o


CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o: CMakeFiles/snappy.dir/flags.make
CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o: ../snappy-stubs-internal.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/u/s/h/sharath/private/cs739/snappy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o -c /u/s/h/sharath/private/cs739/snappy/snappy-stubs-internal.cc

CMakeFiles/snappy.dir/snappy-stubs-internal.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/snappy.dir/snappy-stubs-internal.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /u/s/h/sharath/private/cs739/snappy/snappy-stubs-internal.cc > CMakeFiles/snappy.dir/snappy-stubs-internal.cc.i

CMakeFiles/snappy.dir/snappy-stubs-internal.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/snappy.dir/snappy-stubs-internal.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /u/s/h/sharath/private/cs739/snappy/snappy-stubs-internal.cc -o CMakeFiles/snappy.dir/snappy-stubs-internal.cc.s

CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.requires:

.PHONY : CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.requires

CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.provides: CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.requires
	$(MAKE) -f CMakeFiles/snappy.dir/build.make CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.provides.build
.PHONY : CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.provides

CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.provides.build: CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o


CMakeFiles/snappy.dir/snappy.cc.o: CMakeFiles/snappy.dir/flags.make
CMakeFiles/snappy.dir/snappy.cc.o: ../snappy.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/u/s/h/sharath/private/cs739/snappy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/snappy.dir/snappy.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/snappy.dir/snappy.cc.o -c /u/s/h/sharath/private/cs739/snappy/snappy.cc

CMakeFiles/snappy.dir/snappy.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/snappy.dir/snappy.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /u/s/h/sharath/private/cs739/snappy/snappy.cc > CMakeFiles/snappy.dir/snappy.cc.i

CMakeFiles/snappy.dir/snappy.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/snappy.dir/snappy.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /u/s/h/sharath/private/cs739/snappy/snappy.cc -o CMakeFiles/snappy.dir/snappy.cc.s

CMakeFiles/snappy.dir/snappy.cc.o.requires:

.PHONY : CMakeFiles/snappy.dir/snappy.cc.o.requires

CMakeFiles/snappy.dir/snappy.cc.o.provides: CMakeFiles/snappy.dir/snappy.cc.o.requires
	$(MAKE) -f CMakeFiles/snappy.dir/build.make CMakeFiles/snappy.dir/snappy.cc.o.provides.build
.PHONY : CMakeFiles/snappy.dir/snappy.cc.o.provides

CMakeFiles/snappy.dir/snappy.cc.o.provides.build: CMakeFiles/snappy.dir/snappy.cc.o


# Object files for target snappy
snappy_OBJECTS = \
"CMakeFiles/snappy.dir/snappy-c.cc.o" \
"CMakeFiles/snappy.dir/snappy-sinksource.cc.o" \
"CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o" \
"CMakeFiles/snappy.dir/snappy.cc.o"

# External object files for target snappy
snappy_EXTERNAL_OBJECTS =

libsnappy.a: CMakeFiles/snappy.dir/snappy-c.cc.o
libsnappy.a: CMakeFiles/snappy.dir/snappy-sinksource.cc.o
libsnappy.a: CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o
libsnappy.a: CMakeFiles/snappy.dir/snappy.cc.o
libsnappy.a: CMakeFiles/snappy.dir/build.make
libsnappy.a: CMakeFiles/snappy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/u/s/h/sharath/private/cs739/snappy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX static library libsnappy.a"
	$(CMAKE_COMMAND) -P CMakeFiles/snappy.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/snappy.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/snappy.dir/build: libsnappy.a

.PHONY : CMakeFiles/snappy.dir/build

CMakeFiles/snappy.dir/requires: CMakeFiles/snappy.dir/snappy-c.cc.o.requires
CMakeFiles/snappy.dir/requires: CMakeFiles/snappy.dir/snappy-sinksource.cc.o.requires
CMakeFiles/snappy.dir/requires: CMakeFiles/snappy.dir/snappy-stubs-internal.cc.o.requires
CMakeFiles/snappy.dir/requires: CMakeFiles/snappy.dir/snappy.cc.o.requires

.PHONY : CMakeFiles/snappy.dir/requires

CMakeFiles/snappy.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/snappy.dir/cmake_clean.cmake
.PHONY : CMakeFiles/snappy.dir/clean

CMakeFiles/snappy.dir/depend:
	cd /u/s/h/sharath/private/cs739/snappy/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /u/s/h/sharath/private/cs739/snappy /u/s/h/sharath/private/cs739/snappy /u/s/h/sharath/private/cs739/snappy/build /u/s/h/sharath/private/cs739/snappy/build /u/s/h/sharath/private/cs739/snappy/build/CMakeFiles/snappy.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/snappy.dir/depend

