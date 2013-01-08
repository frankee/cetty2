#

protobuf_generate_service(PROTO_SRCS echo.proto)
message(STATUS "craft echo example using ${PROTO_SRCS}")

cxx_executable_current_path_no_link(CraftEchoServer ${PROTO_SRCS})

if (GPERFTOOLS_LIBRARY_DIRS)
    cxx_link_with_pb(CraftEchoServer tcmalloc ${CRAFT_PROJECT_DEPENDENCIES})
else (GPERFTOOLS_LIBRARY_DIRS)
    cxx_link_with_pb(CraftEchoServer ${CRAFT_PROJECT_DEPENDENCIES})
endif (GPERFTOOLS_LIBRARY_DIRS)
