#

protobuf_generate_service(PROTO_SRCS echo.proto)
message(STATUS "craft echo example using ${PROTO_SRCS}")

cxx_executable_current_path_no_link(CraftEchoServer ${PROTO_SRCS})
cxx_link_with_pb(CraftEchoServer tcmalloc ${CRAFT_PROJECT_DEPENDENCIES})
