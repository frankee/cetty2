# Where cetty's .h files can be found.
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/cetty-craft/src
)

ADD_SUBDIRECTORY(src)

SET(CRAFT_PROJECT_DEPENDENCIES cetty-craft cetty-protobuf-serialization cetty-protobuf cetty-service cetty-config cetty yaml-cpp)

########################################################################
#
# Samples on how to link user tests with gtest or gtest_main.
#
# They are not built by default.  To build them, set the
# build_gtest_samples option to ON.  You can do it by running ccmake
# or specifying the -Dbuild_gtest_samples=ON flag when running cmake.
#option(BUILD_CETTY_CRAFT_EXAMPLES "Build craft example programs." ON)

#if (BUILD_CETTY_CRAFT_EXAMPLES)
	MESSAGE(STATUS "BUILDING CRAFT SAMPLES...")
	ADD_SUBDIRECTORY(example)
#endif()

