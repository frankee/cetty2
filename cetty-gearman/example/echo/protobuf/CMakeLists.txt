GENERATE_SERVICE(PROTO_SRCS echo.proto)
message(STATUS "gearman protobuf echo example using ${PROTO_SRCS}")

SET(PB_DEPENDENCIES cetty-gearman cetty-protobuf-serialization cetty-protobuf cetty-service cetty-config cetty yaml-cpp)

cxx_executable_current_path_no_link(GearmanProtobufEchoWorker ${PROTO_SRCS})

if (GPERFTOOLS_LIBRARY)
    cxx_link_with_pb(GearmanProtobufEchoWorker ${GPERFTOOLS_LIBRARY} ${PB_DEPENDENCIES})
else (GPERFTOOLS_LIBRARY)
    cxx_link_with_pb(GearmanProtobufEchoWorker ${PB_DEPENDENCIES})
endif (GPERFTOOLS_LIBRARY)

cxx_executable_current_path_no_link(GearmanProtobufEchoClient ${PROTO_SRCS})

if (GPERFTOOLS_LIBRARY)
    cxx_link_with_pb(GearmanProtobufEchoClient ${GPERFTOOLS_LIBRARY} ${PB_DEPENDENCIES})
else (GPERFTOOLS_LIBRARY)
    cxx_link_with_pb(GearmanProtobufEchoClient ${PB_DEPENDENCIES})
endif (GPERFTOOLS_LIBRARY)
