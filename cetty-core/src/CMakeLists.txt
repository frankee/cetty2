# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/bootstrap BOOTSTRAP_DIR)
AUX_SOURCE_DIRECTORY(cetty/buffer BUFFER_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel CHANNEL_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel/socket/asio CHANNEL_ASIO_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/frame HANDLER_FRAME_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/http HANDLER_HTTP_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/oneone HANDLER_ONEONE_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/replay HANDLER_REPLAY_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/logging HANDLER_LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/timeout HANDLER_TIMEOUT_DIR)
AUX_SOURCE_DIRECTORY(cetty/logging LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/cetty/util/internal/asio UTIL_INTERNAL_ASIO_DIR)
AUX_SOURCE_DIRECTORY(cetty/cetty/util/internal UTIL_INTERNAL_DIR)
AUX_SOURCE_DIRECTORY(cetty/cetty/util UTIL_DIR)

SET(libsources ${BOOTSTRAP_DIR} ${BUFFER_DIR} ${CHANNEL_ASIO_DIR} ${CHANNEL_DIR} ${HANDLER_FRAME_DIR} ${HANDLER_HTTP_DIR} 
	${HANDLER_ONEONE_DIR} ${HANDLER_REPLAY_DIR} ${STRING_DIR} ${HANDLER_LOGGING_DIR} ${HANDLER_TIMEOUT_DIR} ${LOGGING_DIR} ${UTIL_INTERNAL_ASIO_DIR} ${UTIL_INTERNAL_DIR} ${UTIL_DIR})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
cxx_static_library(cetty "${cxx_default}" ${libsources})
