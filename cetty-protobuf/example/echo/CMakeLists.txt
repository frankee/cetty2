SET(SAMPLE_DEPENDENCIES protobufservice cettyservice cettyconfig cetty ${PROTOBUF_LIBRARIES})

GENERATE_SERVICE(PROTO_SRCS echo.proto)

cxx_executable_current_path_no_link(ProtobufEchoClient ${PROTO_SRCS})
cxx_link_pb(ProtobufEchoClient cetty-protobuf-serialization cetty-protobuf
cetty-service cetty-config cetty yaml-cpp)

cxx_executable_current_path_no_link(ProtobufEchoServer ${PROTO_SRCS})
cxx_link_pb(ProtobufEchoServer cetty-protobuf-serialization cetty-protobuf
cetty-service cetty-config cetty yaml-cpp)
