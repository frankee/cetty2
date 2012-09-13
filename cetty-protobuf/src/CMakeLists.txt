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
target_link_libraries(ProtobufServiceGenerator protoc protobuf ${Boost_LIBRARIES})
#ADD_DEPENDENCIES(ProtobufServiceGenerator protoc protobuf)

PROTOBUF_GENERATE_RPC(PROTO_SRCS cetty/protobuf/service/service.proto)

SET(libpbservicesources ${SERVICE_DIR} ${BUILDER_DIR} ${HANDLER_DIR})
SET(libpbserializationsources ${SERIALIZATION_DIR} ${JSON_DIR} ${XML_DIR} ${PNG_DIR})

cxx_static_library(protobufservice "${cxx_default}" ${libpbservicesources})
cxx_static_library(protobufserialization "${cxx_default}" ${libpbserializationsources})
