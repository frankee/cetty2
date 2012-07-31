# Where cetty's .h files can be found.
#INCLUDE_DIRECTORIES(
#  ${PROJECT_SOURCE_DIR}/cetty-gearman/src
#)

ADD_SUBDIRECTORY(src)
option(BUILD_CETTY_GEARMAN_EXAMPLES "Build sample programs." ON)
########################################################################
#
# Samples on how to link user tests with gtest or gtest_main.
#
# They are not built by default.  To build them, set the
# build_gtest_samples option to ON.  You can do it by running ccmake
# or specifying the -Dbuild_gtest_samples=ON flag when running cmake.
#option(BUILD_CETTY_REDIS_EXAMPLES "Build example programs." OFF)
if (BUILD_CETTY_GEARMAN_EXAMPLES)
	MESSAGE(STATUS "BUILDING SAMPLES...")
	ADD_SUBDIRECTORY(sample)
endif()
