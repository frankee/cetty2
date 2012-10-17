SET(SAMPLE_DEPENDENCIES protobufservice cettyservice cettyconfig cetty ${PROTOBUF_LIBRARIES})

protobuf_generate_rpc(PROTO_SRCS echo.proto)

cxx_executable_current_path_no_link(client ${PROTO_SRCS})
target_link_libraries(client ${SAMPLE_DEPENDENCIES})

cxx_executable_current_path_no_link(server ${PROTO_SRCS})
target_link_libraries(server ${SAMPLE_DEPENDENCIES})
