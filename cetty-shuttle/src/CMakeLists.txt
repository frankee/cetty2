# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/shuttle SHUTTLE_DIR)

AUX_SOURCE_DIRECTORY(cetty/shuttle/protobuf SHUTTLE_PROTOBUF_DIR)

GENERATE_CONFIG(SHUTTLE_CONF_SRCS SHUTTLE_CONF_HDRS cetty/shuttle/ShuttleConfig.proto)

SET(shiro_sources ${SHUTTLE_DIR} ${SHUTTLE_PROTOBUF_DIR} ${SHUTTLE_CONF_SRCS})
SET(shiro_lib cetty-shiro)

SET(SHUTTLE_BIN Shuttle)
cxx_executable_no_link(${SHUTTLE_BIN} ${shiro_sources})
cxx_link_craft(${SHUTTLE_BIN})

#install(TARGETS ${ZURG_LIB} DESTINATION lib)
install(TARGETS ${SHUTTLE_BIN} DESTINATION bin)
#install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/zurg/ DESTINATION include/cetty/zurg)
