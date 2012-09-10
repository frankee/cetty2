# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/protobuf/service SERVICE_DIR)

AUX_SOURCE_DIRECTORY(cetty/protobuf/service/builder BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/generator GENERATOR_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/service/handler HANDLER_DIR)

AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization SERIALIZATION_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/json )
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/xml XML_DIR)
AUX_SOURCE_DIRECTORY(cetty/protobuf/serialization/png PNG_DIR)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(ProtobufGenerator protoc protobuf ${GENERATOR_DIR})
ADD_DEPENDENCIES(proto-generator protoc protobuf)


function(PROTOBUF_GENERATE_RPC SRCS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_RPC() called without any proto files")
    return()
  endif(NOT ARGN)

  set(${SRCS})
  foreach(FIL ${ARGN})
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")

    add_custom_command(
      OUTPUT "${FIL_WE}.pb.cc" "${FIL_WE}.pb.h"
      COMMAND ${PROJECT_BINARY_DIR}/bin/proto-generator
      ARGS --rpc_out . ${CMAKE_CURRENT_SOURCE_DIR}/${FIL} -I${CMAKE_CURRENT_SOURCE_DIR} -I${PROJECT_SOURCE_DIR} -I${PROTOBUF_INCLUDE_DIR}
      MAIN_DEPENDENCY ${FIL}
      VERBATIM )
  endforeach()
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
endfunction()

PROTOBUF_GENERATE_RPC(PROTO_SRCS cetty/protobuf/service/service.proto cetty/protobuf/service/common.proto)

SET(libservicesources ${SERVICE_DIR} ${BUILDER_DIR} ${HANDLER_DIR})

SET(libserializationsources ${SERIALIZATION_DIR} ${JSON_DIR} ${XML_DIR} ${PNG_DIR})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
cxx_static_library(protobufservice "${cxx_default}" ${libservicesources})
cxx_static_library(protobufserialization "${cxx_default}" ${libserializationsources})
