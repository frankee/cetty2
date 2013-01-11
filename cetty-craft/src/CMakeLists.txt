# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/craft CRAFT_DIR)
AUX_SOURCE_DIRECTORY(cetty/craft/builder CRAFT_BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/craft/http CRAFT_HTTP_DIR)

AUX_SOURCE_DIRECTORY(cetty/craft/access CRAFT_ACCESS_DIR)

SET(craft_sources ${CRAFT_DIR} ${CRAFT_BUILDER_DIR} ${CRAFT_HTTP_DIR})
SET(craft_lib cetty-craft)

GENERATE_SERVICE_separate(PROTO_SRCS PROTO_HDRS cetty/craft/craft.proto  cetty/craft/craft_options.proto)
#message(STATUS "generate source files ${PROTO_SRCS}")

GENERATE_SERVICE_separate(PROTO_ACCESS_SRCS PROTO_ACCESS_HDRS cetty/craft/access/AccessControl.proto)
message(STATUS "generate source files ${PROTO_ACCESS_SRCS}")

cxx_static_library(${craft_lib} ${craft_sources} ${PROTO_SRCS})

install(TARGETS ${craft_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/craft/ DESTINATION include/cetty/craft)
