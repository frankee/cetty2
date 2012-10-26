########################################################################
#
# Project-wide settings

# Name of the project.
#
# CMake files in this project can refer to the root source directory
# as ${gtest_SOURCE_DIR} and to the root binary directory as
# ${gtest_BINARY_DIR}.
# Language "C" is required for find_package(Threads).
PROJECT(CETTY CXX C)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/cmake)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

if (MSVC)
  # For MSVC, CMake sets certain flags to defaults we want to override.
  # This replacement code is taken from sample in the CMake Wiki at
  # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
  foreach (flag_var
           CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
           CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    # In hermetic build environments, tests may not have access to MS runtime
    # DLLs, so this replaces /MD (CRT libraries in DLLs) with /MT (static CRT
    # libraries).
    #string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
    # We prefer more strict warning checking for building Google Test.
    # Replaces /W3 with /W4 in defaults.
    string(REPLACE "/W3" "-W4" ${flag_var} "${${flag_var}}")
  endforeach()
endif()

# Where cetty's .h files can be found.
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/cetty-core/src
)

# Where the cetty libraries can be found.
LINK_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_BINARY_DIR}/lib)

set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.48.0 COMPONENTS date_time system thread filesystem regex program_options REQUIRED )

INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
LINK_DIRECTORIES(${BOOST_LIB_DIRS})


# Defines CMAKE_USE_PTHREADS_INIT and CMAKE_THREAD_LIBS_INIT.
FIND_PACKAGE(Threads)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
LINK_DIRECTORIES(${PROTOBUF_LIB_DIRS})

find_package(YamlCpp REQUIRED)
include_directories(${YAMLCPP_INCLUDE_DIRS})
LINK_DIRECTORIES(${YAMLCPP_LIBRARY_DIRS})

find_package(Soci)
include_directories(${SOCI_INCLUDE_DIRS})
LINK_DIRECTORIES($(SOCI_LIBRARY))

# Defines the compiler/linker flags used to build cetty.  You can
# tweak these definitions to suit your need.  A variable's value is
# empty before it's explicitly assigned to.

if (MSVC)
  # Newlines inside flags variables break CMake's NMake generator.
  #set(cxx_base_flags "-GS -W4 -WX -wd4275 -nologo -J -Zi")
  set(cxx_base_flags "-GS -W3 -wd4275 -wd4996 -nologo -Zi")
  set(cxx_base_flags "${cxx_base_flags} -D_UNICODE -DUNICODE -DWIN32 -D_WIN32 -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0501")
  #set(cxx_base_flags "${cxx_base_flags} -DSTRICT")
  set(cxx_base_flags "${cxx_base_flags} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -Dinline=__inline")
  set(cxx_exception_flags "-EHsc -D_HAS_EXCEPTIONS=1")
  set(cxx_no_exception_flags "-D_HAS_EXCEPTIONS=0")
  set(cxx_no_rtti_flags "-GR-")
elseif (CMAKE_COMPILER_IS_GNUCXX)
  set(cxx_base_flags "-Wall -Wno-shadow")
  set(cxx_exception_flags "-fexceptions")
  set(cxx_no_exception_flags "-fno-exceptions")
  # Until version 4.3.2, GCC doesn't define a macro to indicate
  # whether RTTI is enabled.  Therefore we define CETTY _HAS_RTTI
  # explicitly.
  set(cxx_no_rtti_flags "-fno-rtti -DCETTY_HAS_RTTI=0")
  set(cxx_strict_flags "-Wextra")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "SunPro")
  set(cxx_exception_flags "-features=except")
  # Sun Pro doesn't provide macros to indicate whether exceptions and
  # RTTI are enabled, so we define CETTY_HAS_* explicitly.
  set(cxx_no_exception_flags "-features=no%except -DCETTY_HAS_EXCEPTIONS=0")
  set(cxx_no_rtti_flags "-features=no%rtti -DCETTY_HAS_RTTI=0")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "VisualAge" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "XL")
  # CMake 2.8 changes Visual Age's compiler ID to "XL".
  set(cxx_exception_flags "-qeh")
  set(cxx_no_exception_flags "-qnoeh")
  # Until version 9.0, Visual Age doesn't define a macro to indicate
  # whether RTTI is enabled.  Therefore we define CETTY_HAS_RTTI
  # explicitly.
  set(cxx_no_rtti_flags "-qnortti -DCETTY_HAS_RTTI=0")
endif()

if (CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  set(cxx_base_flags "${cxx_base_flags} -DCETTY_HAS_PTHREAD=1")
endif()

# For building cetty's own tests and samples.
set(cxx_exception "${CMAKE_CXX_FLAGS} ${cxx_base_flags} ${cxx_exception_flags}")
set(cxx_no_exception
    "${CMAKE_CXX_FLAGS} ${cxx_base_flags} ${cxx_no_exception_flags}")
set(cxx_default "${cxx_exception}")
set(cxx_no_rtti "${cxx_default} ${cxx_no_rtti_flags}")

# For building the cetty libraries.
set(cxx_strict "${cxx_default} ${cxx_strict_flags}")


if (CMAKE_COMPILER_IS_GNUCXX)
  set(CXX_FLAGS
    -g
    # -DVALGRIND
    -D_FILE_OFFSET_BITS=64
    -Wall
    #-Wextra
    #-Werror
    -Wconversion
    -Wno-unused-parameter
    #-Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    #-Wshadow
    -Wwrite-strings
    #-march=native
    # -MMD
    # -std=c++0x
    -rdynamic
    )
  if(CMAKE_BUILD_BITS EQUAL 32)
    list(APPEND CXX_FLAGS "-m32")
  endif()
  string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

  set(CMAKE_CXX_FLAGS_DEBUG "-O0")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
endif()

########################################################################
#
# Defines the cetty libraries.
function(cxx_library_with_type name type cxx_flags)
  # type can be either STATIC or SHARED to denote a static or shared library.
  # ARGN refers to additional arguments after 'cxx_flags'.
  add_library(${name} ${type} ${ARGN})
  if(cxx_flags)
    message(STATUS "cxx_library_with_type setting the cxx_flags ${cxx_flags}")
    set_target_properties(${name}
      PROPERTIES
      COMPILE_FLAGS "${cxx_flags}")
  endif()

  if (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
  elseif (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${Boost_LIBRARIES})
  endif()
endfunction()

function(cxx_static_library name)
  cxx_library_with_type(${name} STATIC 0 ${ARGN})
endfunction()

function(cxx_shared_library name)
  cxx_library_with_type(${name} SHARED 0 ${ARGN})
endfunction()


########################################################################
#
# Samples on how to link user tests with gtest or gtest_main.
#
# They are not built by default.  To build them, set the
# build_gtest_samples option to ON.  You can do it by running ccmake
# or specifying the -Dbuild_gtest_samples=ON flag when running cmake.
#option(BUILD_EXAMPLES "Build example programs." OFF)

# cxx_executable_with_flags(name cxx_flags lib srcs...)
#
# creates a named C++ executable that depends on the given library and
# is built from the given source files with the given compiler flags.
function(cxx_executable_with_flags name cxx_flags lib)
  add_executable(${name} ${ARGN})
  if (cxx_flags)
    message(STATUS "cxx_executable_with_flags setting the cxx_flags ${cxx_flags}")
    set_target_properties(${name}
      PROPERTIES
      COMPILE_FLAGS "${cxx_flags}")
  endif()
  target_link_libraries(${name} ${lib})
endfunction()

function(cxx_executable_with_flags_no_link name cxx_flags)
  add_executable(${name} ${ARGN})
  if (cxx_flags)
    message(STATUS "cxx_executable_with_flags_no_link setting the cxx_flags ${cxx_flags}")
    set_target_properties(${name}
      PROPERTIES
      COMPILE_FLAGS "${cxx_flags}")
  endif()
endfunction()

# cxx_executable(name dir lib srcs...)
#
# creates a named target that depends on the given lib and is built
# from the given source files.  dir/name.cc is implicitly included in
# the source file list.
function(cxx_executable name dir lib)
  cxx_executable_with_flags(${name} 0 ${lib} "${dir}/${name}.cpp" ${ARGN})
endfunction()

function(cxx_executable_no_link name dir)
  cxx_executable_with_flags_no_link(${name} 0 "${dir}/${name}.cpp" ${ARGN})
endfunction()

function(cxx_executable_current_path name lib)
  cxx_executable_with_flags(${name} 0 ${lib} "${name}.cpp" ${ARGN})
endfunction()

function(cxx_executable_current_path_no_link name)
  cxx_executable_with_flags_no_link(${name} 0 "${name}.cpp" ${ARGN})
endfunction()

function(cxx_link name)
  if (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${ARGN})
  elseif (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${Boost_LIBRARIES} ${ARGN})
  endif()
endfunction()

function(cxx_link_with_pb name)
  if (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${ARGN} ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
  elseif (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${ARGN} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES})
  endif()
endfunction()

function(PROTOBUF_GENERATE_SERVICE SRCS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_SERVICE() called without any proto files")
    return()
  endif(NOT ARGN)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

  set(${SRCS})
  foreach(FIL ${ARGN})
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h"
      COMMAND ${PROJECT_BINARY_DIR}/bin/ProtobufServiceGenerator
      ARGS ./${FIL} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I. -I${PROJECT_INCLUDE_DIR} -I${PROTOBUF_INCLUDE_DIR}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      MAIN_DEPENDENCY ${FIL}
      VERBATIM )
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
endfunction()

function(PROTOBUF_GENERATE_SERVICE_SEPARATE SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_SERVICE_SEPARATE() called without any proto files")
    return()
  endif(NOT ARGN)

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(FIL_PATH ${FIL} PATH)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(ABS_FILE ${FIL} ABSOLUTE)

    set(OUT_HEADER "${PROJECT_INCLUDE_DIR}/${FIL_PATH}/${FIL_WE}.pb.h")
    set(OUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FIL_PATH}/${FIL_WE}.pb.cc")

    list(APPEND ${HDRS} ${OUT_HEADER})
    list(APPEND ${SRCS} ${OUT_SOURCE})

    add_custom_command(
      OUTPUT ${OUT_HEADER} ${OUT_SOURCE}
      COMMAND ${PROJECT_BINARY_DIR}/bin/ProtobufServiceGenerator
      ARGS ${FIL} --cpp_out=. --src_out=${CMAKE_CURRENT_BINARY_DIR} -I. -I${PROTOBUF_INCLUDE_DIR}
      WORKING_DIRECTORY ${PROJECT_INCLUDE_DIR}
      MAIN_DEPENDENCY ${ABS_FIL}
      VERBATIM)
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "debug")
  message(STATUS "you do not set the CMAKE_BUILD_TYPE, using debug type.")
endif()

ADD_SUBDIRECTORY(cetty-core)

if (YAMLCPP_INCLUDE_DIRS)
  ADD_SUBDIRECTORY(cetty-config)  #auto generator depends on protobuf
  SET(HAS_CETTY_CONFIG 1)
else ()
  message(STATUS "has not found yaml-cpp, skip cetty-config module")
endif()

ADD_SUBDIRECTORY(cetty-service)
ADD_SUBDIRECTORY(cetty-redis)
ADD_SUBDIRECTORY(cetty-shiro)

ADD_SUBDIRECTORY(cetty-protobuf)
ADD_SUBDIRECTORY(cetty-gearman)

ADD_SUBDIRECTORY(cetty-craft)

#
# Core installation
#

file(GLOB CETTY_CORE_HEADERS *.h)
install(FILES ${CETTY_CORE_HEADERS} DESTINATION
	${INCLUDEDIR}/cetty)
#install(TARGETS ${LIBRARY_OUTPUT_PATH} 
#    RUNTIME DESTINATION ${BINDIR}
#    LIBRARY DESTINATION ${LIBDIR}
#    ARCHIVE DESTINATION ${LIBDIR})

