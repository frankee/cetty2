# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/protobuf/service SERVICE_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/builder BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/handler HANDLER_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/http HTTP_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/http/map HTTP_MAP_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/proto PROTO_DIR)

SET(libsources ${SERVICE_DIR} ${BUILDER_DIR} ${HANDLER_DIR} ${HTTP_DIR} ${HTTP_MAP_DIR} ${PROTO_DIR})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
cxx_static_library(protobufservice "${cxx_default}" ${libsources})
