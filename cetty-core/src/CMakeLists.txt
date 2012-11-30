# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/bootstrap BOOTSTRAP_DIR)
AUX_SOURCE_DIRECTORY(cetty/buffer BUFFER_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel CHANNEL_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel/asio CHANNEL_ASIO_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec HANDLER_CODEC_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/http HANDLER_HTTP_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/logging HANDLER_LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/timeout HANDLER_TIMEOUT_DIR)
AUX_SOURCE_DIRECTORY(cetty/logging LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/util UTIL_DIR)


SET(cetty_sources ${BOOTSTRAP_DIR} ${BUFFER_DIR} ${CHANNEL_ASIO_DIR}
  ${CHANNEL_DIR} ${HANDLER_CODEC_DIR} ${HANDLER_HTTP_DIR}
  ${HANDLER_LOGGING_DIR} ${HANDLER_TIMEOUT_DIR} ${LOGGING_DIR} ${UTIL_DIR})
SET(cetty_lib cetty)

cxx_static_library(${cetty_lib} ${cetty_sources})

install(TARGETS ${cetty_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/bootstrap/ DESTINATION include/cetty/bootstrap)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/buffer/ DESTINATION include/cetty/buffer)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/channel/ DESTINATION include/cetty/channel)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/handler/ DESTINATION include/cetty/handler)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/logging/ DESTINATION include/cetty/logging)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/util/ DESTINATION include/cetty/util)
install(FILES
   ${PROJECT_INCLUDE_DIR}/cetty/Common.h
   ${PROJECT_INCLUDE_DIR}/cetty/Platform.h
   ${PROJECT_INCLUDE_DIR}/cetty/PlatformWin32.h
   ${PROJECT_INCLUDE_DIR}/cetty/Types.h
   ${PROJECT_INCLUDE_DIR}/cetty/Version.h
   DESTINATION include/cetty)
