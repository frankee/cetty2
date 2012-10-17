# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/protobuf/service SERVICE_DIR)

AUX_SOURCE_DIRECTORY(cetty/protobuf/service/builder BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/handler HANDLER_DIR)

AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization SERIALIZATION_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/json JSON_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/xml XML_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/png PNG_DIR)

SET(GENERATOR_DIR cetty/protobuf/service/generator)

cxx_executable_no_link(ProtobufServiceGenerator ${GENERATOR_DIR})
cxx_link_with_pb(ProtobufServiceGenerator protoc)

protobuf_generate_rpc(PROTO_SRCS cetty/protobuf/service/service.proto)

SET(pbservicesources ${SERVICE_DIR} ${BUILDER_DIR} ${HANDLER_DIR})
SET(pbserializationsources ${SERIALIZATION_DIR} ${JSON_DIR} ${XML_DIR} ${PNG_DIR})

cxx_static_library(protobufservice ${pbservicesources})
cxx_static_library(protobufserialization ${pbserializationsources})

install(TARGETS protobufservice DESTINATION lib)
install(TARGETS protobufserialization DESTINATION lib)
install(TARGETS ProtobufServiceGenerator DESTINATION bin)