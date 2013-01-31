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
    target_link_libraries(${name} ${ARGN} ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES} z)
  elseif (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${ARGN} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} z)
  endif()
endfunction()

function(cxx_link_craft name)
  if (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${ARGN} ${CETTY_LIBRARYS} ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES} ${YAMLCPP_LIBRARY} z)
  elseif (CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(${name} ${ARGN} ${CETTY_LIBRARYS} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${YAMLCPP_LIBRARY} z)
  endif()
endfunction()

function(GENERATE_SERVICE SRCS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_SERVICE() called without any proto files")
    return()
  endif(NOT ARGN)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

  set(${SRCS})
  foreach(FIL ${ARGN})
	get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(ABS_FILE ${FIL} ABSOLUTE)

	set(GEN_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${FIL_WE}.pb.h")
	set(OUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")
	set(GEN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${FIL_WE}.pb.cc")
    set(OUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
	
	list(APPEND ${SRCS} ${OUT_SOURCE})
	
    add_custom_command(
      OUTPUT ${OUT_SOURCE} ${OUT_HEADER}
      COMMAND ${CETTY_SERVICE_GENERATOR}
	  ARGS ${FIL} --service_out=. -I. -I${PROJECT_INCLUDE_DIR} -I${PROTOBUF_INCLUDE_DIR} -I${CETTY_INCLUDE_DIR} 
	  COMMAND mv
	  ARGS ${GEN_HEADER} ${OUT_HEADER}
	  COMMAND mv
	  ARGS ${GEN_SOURCE} ${OUT_SOURCE}
	  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      MAIN_DEPENDENCY ${ABS_FILE}
      VERBATIM )
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
endfunction()

function(GENERATE_SERVICE_SEPARATE SRCS HDRS)
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
	set(GEN_SOURCE "${PROJECT_INCLUDE_DIR}/${FIL_PATH}/${FIL_WE}.pb.cc")
    set(OUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")

    list(APPEND ${HDRS} ${OUT_HEADER})
    list(APPEND ${SRCS} ${OUT_SOURCE})

    add_custom_command(
      OUTPUT ${OUT_HEADER} ${OUT_SOURCE}
	  COMMAND ${CETTY_SERVICE_GENERATOR}
	  ARGS ${FIL} --service_out=. -I. -I${PROJECT_INCLUDE_DIR} -I${PROTOBUF_INCLUDE_DIR} -I${CETTY_INCLUDE_DIR}
	  COMMAND mv
	  ARGS ${GEN_SOURCE} ${OUT_SOURCE}
      WORKING_DIRECTORY ${PROJECT_INCLUDE_DIR}
      MAIN_DEPENDENCY ${ABS_FIL}
      VERBATIM)
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

function(GENERATE_PROTOBUF SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: GENERATE_PROTOBUF() called without any proto files")
    return()
  endif(NOT ARGN)

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(FIL_PATH ${FIL} PATH)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(ABS_FILE ${FIL} ABSOLUTE)

    set(OUT_HEADER "${PROJECT_INCLUDE_DIR}/${FIL_PATH}/${FIL_WE}.pb.h")
	set(GEN_SOURCE "${PROJECT_INCLUDE_DIR}/${FIL_PATH}/${FIL_WE}.pb.cc")
    set(OUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")

    list(APPEND ${HDRS} ${OUT_HEADER})
    list(APPEND ${SRCS} ${OUT_SOURCE})

    add_custom_command(
      OUTPUT ${OUT_HEADER} ${OUT_SOURCE}
	  COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
	  ARGS ${FIL} --cpp_out=. -I. -I${PROJECT_INCLUDE_DIR} -I${PROTOBUF_INCLUDE_DIR} -I${CETTY_INCLUDE_DIR}
	  COMMAND mv
	  ARGS ${GEN_SOURCE} ${OUT_SOURCE}
      WORKING_DIRECTORY ${PROJECT_INCLUDE_DIR}
      MAIN_DEPENDENCY ${ABS_FIL}
      VERBATIM)
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


function(GENERATE_CONFIG SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: GENERATE_CONFIG() called without any proto files")
    return()
  endif(NOT ARGN)

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(FIL_PATH ${FIL} PATH)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(ABS_FILE ${FIL} ABSOLUTE)

    set(OUT_HEADER "${PROJECT_INCLUDE_DIR}/${FIL_PATH}/${FIL_WE}.cnf.h")
	set(GEN_SOURCE "${PROJECT_INCLUDE_DIR}/${FIL_PATH}/${FIL_WE}.cnf.cpp")
    set(OUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.cnf.cpp")

    list(APPEND ${HDRS} ${OUT_HEADER})
    list(APPEND ${SRCS} ${OUT_SOURCE})

    add_custom_command(
      OUTPUT ${OUT_HEADER} ${OUT_SOURCE}
	  COMMAND ${CETTY_CONFIG_GENERATOR}
	  ARGS ${FIL} --config_out=. -I. -I${PROJECT_INCLUDE_DIR} -I${PROTOBUF_INCLUDE_DIR} -I${CETTY_INCLUDE_DIR}
	  COMMAND mv
	  ARGS ${GEN_SOURCE} ${OUT_SOURCE}
      WORKING_DIRECTORY ${PROJECT_INCLUDE_DIR}
      MAIN_DEPENDENCY ${ABS_FIL}
      VERBATIM)
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()
