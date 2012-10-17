#

protobuf_generate_rpc(PROTO_SRCS echo.proto)

cxx_executable_current_path_no_link(server ${PROTO_SRCS})
cxx_link_with_pb(server ${CRAFT_PROJECT_DEPENDENCIES})
