GENERATE_SERVICE(PROTO_SRCS echo.proto)
message(STATUS "gearman protobuf echo example using ${PROTO_SRCS}")

cxx_executable_current_path_no_link(GearmanProtobufEchoWorker ${PROTO_SRCS})
cxx_link_gearman(GearmanProtobufEchoWorker)

cxx_executable_current_path_no_link(GearmanProtobufEchoClient ${PROTO_SRCS})
cxx_link_gearman(GearmanProtobufEchoClient)
