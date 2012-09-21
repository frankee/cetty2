# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/craft CRAFT_DIR)

AUX_SOURCE_DIRECTORY(cetty/craft/builder CRAFT_BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/craft/http CRAFT_HTTP_DIR)

AUX_SOURCE_DIRECTORY(cetty/craft/startup CRAFT_STARTUP_DIR)
AUX_SOURCE_DIRECTORY(cetty/craft/access CRAFT_ACCESS_DIR)

SET(libcraftsources ${CRAFT_DIR} ${CRAFT_BUILDER_DIR} ${CRAFT_HTTP_DIR})

PROTOBUF_GENERATE_RPC(PROTO_SRCS cetty/craft/craft.proto cetty/craft/access/AccessControl.proto)

cxx_static_library(craft "${cxx_default}" ${libcraftsources})

SET(CRAFT_APP_DEPENDENCIES craft protobufservice protobufserialization cettyservice cettyconfig
cetty protobuf yaml-cpp)

#cxx_executable_no_link(startup ${CRAFT_STARTUP_DIR})
#target_link_libraries(startup ${CRAFT_APP_DEPENDENCIES})

#cxx_executable_no_link(AccessControlServer ${CRAFT_ACCESS_DIR})
#target_link_libraries(AccessControlServer ${CRAFT_APP_DEPENDENCIES})
