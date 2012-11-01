# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/zurg ZURG_DIR)
AUX_SOURCE_DIRECTORY(cetty/zurg/master ZURG_MASTER_DIR)
AUX_SOURCE_DIRECTORY(cetty/zurg/slave ZURG_SLAVE_DIR)

SET(zurg_sources ${ZURG_DIR} ${ZURG_MASTER_DIR} ${ZURG_SLAVE_DIR})
SET(zurg_lib cetty-zurg)

protobuf_generate_service_separate(ZURG_MASTER_ACCESS_SRCS ZURG_MASTER_ACCESS_HDRS cetty/zurg/master/master.proto)
message(STATUS "generate source files ${ZURG_MASTER_ACCESS_SRCS}")
message(STATUS "generate header files ${ZURG_MASTER_ACCESS_HDRS}")


protobuf_generate_service_separate(ZURG_SLAVE_ACCESS_SRCS ZURG_SLAVE_ACCESS_HDRS cetty/zurg/slave/slave.proto)
message(STATUS "generate source files ${ZURG_SLAVE_ACCESS_SRCS}")
message(STATUS "generate header files ${ZURG_SLAVE_ACCESS_HDRS}")

cxx_static_library(${zurg_lib} ${zurg_sources})

install(TARGETS ${zurg_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/zurg/ DESTINATION include/cetty/zurg)
