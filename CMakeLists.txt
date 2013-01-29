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

# Where cetty's .h files can be found.
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/cetty-core/src
)

# Where the cetty libraries can be found.
LINK_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_BINARY_DIR}/lib)
  
include (Dependence)
include (MakeFlags)
include (Functions)

#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE "debug")
#  message(STATUS "you do not set the CMAKE_BUILD_TYPE, using debug type.")
#endif()

ADD_SUBDIRECTORY(cetty-core)

if (YAMLCPP_INCLUDE_DIRS)
  ADD_SUBDIRECTORY(cetty-config)  #auto generator depends on protobuf
  SET(HAS_CETTY_CONFIG 1)
else ()
  message(STATUS "has not found yaml-cpp, skip cetty-config module")
endif()

ADD_SUBDIRECTORY(cetty-service)
ADD_SUBDIRECTORY(cetty-redis)
#ADD_SUBDIRECTORY(cetty-shiro)

ADD_SUBDIRECTORY(cetty-protobuf)
ADD_SUBDIRECTORY(cetty-gearman)
ADD_SUBDIRECTORY(cetty-craft)
ADD_SUBDIRECTORY(cetty-zurg)
