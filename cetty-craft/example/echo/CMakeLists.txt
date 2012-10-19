#

protobuf_generate_service(PROTO_SRCS echo.proto)
message(STATUS "craft echo example using ${PROTO_SRCS}")

cxx_executable_current_path_no_link(server ${PROTO_SRCS})
cxx_link_with_pb(server ${CRAFT_PROJECT_DEPENDENCIES})
