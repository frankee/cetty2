function(cxx_library_with_type name type cxx_flags)
  # type can be either STATIC or SHARED to denote a static or shared library.
  # ARGN refers to additional arguments after 'cxx_flags'.
  add_library(${name} ${type} ${ARGN})
  set_target_properties(${name}
    PROPERTIES
    COMPILE_FLAGS "${cxx_flags}")
    if (CMAKE_USE_PTHREADS_INIT)
      target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
	elseif (CMAKE_USE_PTHREADS_INIT)
	  target_link_libraries(${name} ${Boost_LIBRARIES})
    endif()
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
    set_target_properties(${name}
      PROPERTIES
      COMPILE_FLAGS "${cxx_flags}")
  endif()
  target_link_libraries(${name} ${lib})
endfunction()

function(cxx_executable_current_path name lib)
  cxx_executable_with_flags(
    ${name} "${cxx_default}" ${lib} "${name}.cpp" ${ARGN})
endfunction()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(EchoClient cetty  "EchoClientHandler.cpp")
ADD_DEPENDENCIES(EchoClient cetty)