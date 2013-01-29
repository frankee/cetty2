#

AUX_SOURCE_DIRECTORY(cetty/protobuf/service SERVICE_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/builder BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/handler HANDLER_DIR)

AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization SERIALIZATION_DIR)

if (YAMLCPP_INCLUDE_DIRS)
  AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/json JSON_DIR)
else()
  message(STATUS 'has not found yaml-cpp, do not install protobuf-json seriazation module.')
endif()

AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/xml XML_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/bson BSON_DIR)

SET(GENERATOR_DIR cetty/protobuf/service/generator)

cxx_executable_no_link(ProtobufServiceGenerator ${GENERATOR_DIR} cetty/protobuf/service/generator/ServiceGenerator.cpp)
cxx_link_pb(ProtobufServiceGenerator protoc)

GENERATE_SERVICE_SEPARATE(PROTO_SRCS PROTO_HDS cetty/protobuf/service/service_options.proto cetty/protobuf/service/service.proto)
message(STATUS ${PROTO_HDS})

SET(pbservice_sources ${SERVICE_DIR} ${BUILDER_DIR} ${HANDLER_DIR} ${PROTO_SRCS})
SET(pbservice_lib cetty-protobuf)

SET(pbserialization_sources ${SERIALIZATION_DIR} ${JSON_DIR} ${XML_DIR} ${BSON_DIR})
SET(pbserialization_lib cetty-protobuf-serialization)

cxx_static_library(${pbservice_lib} ${pbservice_sources})
cxx_static_library(${pbserialization_lib} ${pbserialization_sources})

install(TARGETS ${pbservice_lib} DESTINATION lib)
install(TARGETS ${pbserialization_lib} DESTINATION lib)
install(TARGETS ProtobufServiceGenerator DESTINATION bin)

install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/protobuf/ DESTINATION include/cetty/protobuf)
