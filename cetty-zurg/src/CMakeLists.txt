# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/zurg ZURG_DIR)
AUX_SOURCE_DIRECTORY(cetty/zurg/master ZURG_MASTER_DIR)
AUX_SOURCE_DIRECTORY(cetty/zurg/slave ZURG_SLAVE_DIR)

GENERATE_SERVICE_SEPARATE(ZURG_PB_SRCS ZURG_PB_HDRS cetty/zurg/master/master.proto    cetty/zurg/master/master_service.proto cetty/zurg/slave/slave.proto cetty/zurg/slave/slave_service.proto)
message(STATUS "generate source files ${ZURG_PB_SRCS}")
message(STATUS "generate header files ${ZURG_PB_HDRS}")

GENERATE_CONFIG(ZURG_CONF_SRCS ZURG_CONF_HDRS cetty/shuttle/ShuttleConfig.proto)

SET(ZURG_LIB cetty-zurg)
SET(ZURG_SRCS ${ZURG_DIR} ${ZURG_PB_SRCS} ${ZURG_CONF_SRCS} ${ZURG_SLAVE_DIR} ${ZURG_MASTER_DIR})
list(REMOVE_ITEM ZURG_SRCS cetty/zurg/slave/ZurgSlave.cpp)

cxx_static_library(${ZURG_LIB} ${ZURG_SRCS} )

SET(ZURG_SLAVE_BIN ZurgSlave)
cxx_executable_no_link(${ZURG_SLAVE_BIN} cetty/zurg/slave)
cxx_link_craft(${ZURG_SLAVE_BIN} cetty-zurg)

install(TARGETS ${ZURG_LIB} DESTINATION lib)
install(TARGETS ${ZURG_SLAVE_BIN} DESTINATION bin)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/zurg/ DESTINATION include/cetty/zurg)
