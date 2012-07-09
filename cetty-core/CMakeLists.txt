# Where cetty's .h files can be found.
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/cetty-core/src
)



#cxx_static_library(cetty "${cxx_default}" ${libsources})
#target_link_libraries(gtest_main gtest)

ADD_SUBDIRECTORY(src)

########################################################################
#
# Samples on how to link user tests with gtest or gtest_main.
#
# They are not built by default.  To build them, set the
# build_gtest_samples option to ON.  You can do it by running ccmake
# or specifying the -Dbuild_gtest_samples=ON flag when running cmake.
option(BUILD_CETTY_CORE_EXAMPLES "Build cetty-cor example programs." OFF)


if (BUILD_CETTY_CORE_EXAMPLES)
	MESSAGE(STATUS "BUILDING SAMPLES...")
	ADD_SUBDIRECTORY(example)
endif()
