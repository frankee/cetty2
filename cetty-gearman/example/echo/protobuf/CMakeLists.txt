SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#find_package(Protobuf REQUIRED)
#include_directories(${PROTOBUF_INCLUDE_DIRS})
#LINK_DIRECTORIES(${PROTOBUF_LIB_DIRS})

#add_executable(server  "server.cpp" "echo.pb.cc")
#set_target_properties(server PROPERTIES COMPILE_FLAGS "${cxx_default}")
#target_link_libraries(server protobufservice cettyservice cettyconfig cetty ${PROTOBUF_LIBRARIES})

#add_executable(client  "client.cpp" "echo.pb.cc")
#set_target_properties(client PROPERTIES COMPILE_FLAGS "${cxx_default}")
#target_link_libraries(client protobufservice cettyservice cettyconfig cetty ${PROTOBUF_LIBRARIES})
include_directories(. /usr/local/include)

link_directories(/usr/local/lib)


SET(SAMPLE_DEPENDENCIES gearman protobufservice  cettyservice cettyconfig
cetty protobuf)

cxx_executable_current_path_no_link(GearmanProtobufEchoClient "echo.pb.cc")
target_link_libraries(GearmanProtobufEchoClient ${SAMPLE_DEPENDENCIES})
cxx_executable_current_path_no_link(GearmanProtobufEchoWorker "echo.pb.cc")
target_link_libraries(GearmanProtobufEchoWorker ${SAMPLE_DEPENDENCIES})
