# Where cetty's .h files can be found.
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/cetty-zurg/src
)

ADD_SUBDIRECTORY(src)

SET(ZURG_PROJECT_DEPENDENCIES cetty-zurg cetty-protobuf-serialization cetty-protobuf cetty-service cetty-config cetty yaml-cpp)

########################################################################
#
# Samples on how to link user tests with gtest or gtest_main.
#
# They are not built by default.  To build them, set the
# build_gtest_samples option to ON.  You can do it by running ccmake
# or specifying the -Dbuild_gtest_samples=ON flag when running cmake.
#option(BUILD_CETTY_ZURG_EXAMPLES "Build zurg example programs." ON)

#if (BUILD_CETTY_ZURG_EXAMPLES)
    MESSAGE(STATUS "BUILDING ZURG SAMPLES...")
    ADD_SUBDIRECTORY(example)
#endif()