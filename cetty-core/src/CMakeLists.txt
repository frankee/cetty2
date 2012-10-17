# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/bootstrap BOOTSTRAP_DIR)
AUX_SOURCE_DIRECTORY(cetty/buffer BUFFER_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel CHANNEL_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel/socket/asio CHANNEL_ASIO_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec HANDLER_CODEC_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/http HANDLER_HTTP_DIR)
#AUX_SOURCE_DIRECTORY(cetty/handler/logging HANDLER_LOGGING_DIR)
#AUX_SOURCE_DIRECTORY(cetty/handler/timeout HANDLER_TIMEOUT_DIR)
AUX_SOURCE_DIRECTORY(cetty/logging LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/util UTIL_DIR)
AUX_SOURCE_DIRECTORY(cetty/util/internal UTIL_INTERNAL_DIR)
AUX_SOURCE_DIRECTORY(cetty/util/internal/asio UTIL_INTERNAL_ASIO_DIR)


SET(cetty_sources ${BOOTSTRAP_DIR} ${BUFFER_DIR} ${CHANNEL_ASIO_DIR} ${CHANNEL_DIR} ${HANDLER_CODEC_DIR} ${HANDLER_HTTP_DIR} 
${HANDLER_ONEONE_DIR} ${HANDLER_REPLAY_DIR} ${STRING_DIR} ${LOGGING_DIR} ${UTIL_INTERNAL_ASIO_DIR} ${UTIL_INTERNAL_DIR} ${UTIL_DIR})

cxx_static_library(cetty ${cetty_sources})

install(TARGETS cetty DESTINATION lib)
