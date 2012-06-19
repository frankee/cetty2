# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/bootstrap BOOTSTRAP_DIR)
AUX_SOURCE_DIRECTORY(cetty/buffer BUFFER_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel/socket/asio ASIO_DIR)
AUX_SOURCE_DIRECTORY(cetty/channel CHANNEL_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/frame FRAME_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/http HTTP_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/oneone ONEONE_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/replay REPLAY_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/codec/string STRING_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/logging HANDLER_LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/handler/timeout TIMEOUT_DIR)
AUX_SOURCE_DIRECTORY(cetty/logging CETTY_LOGGING_DIR)
AUX_SOURCE_DIRECTORY(cetty/cetty/util/internal/asio UTIL_ASIO_DIR)
AUX_SOURCE_DIRECTORY(cetty/cetty/util/internal INTERNAL_DIR)
AUX_SOURCE_DIRECTORY(cetty/cetty/util UTIL_DIR)

SET(libsources ${BOOTSTRAP_DIR} ${BUFFER_DIR} ${ASIO_DIR} ${CHANNEL_DIR} ${FRAME_DIR} ${HTTP_DIR} 
	${ONEONE_DIR} ${REPLAY_DIR} ${STRING_DIR} ${HANDLER_LOGGING_DIR} ${TIMEOUT_DIR} ${CETTY_LOGGING_DIR} ${UTIL_ASIO_DIR} ${INTERNAL_DIR} ${UTIL_DIR})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
cxx_static_library(cetty "${cxx_default}" ${libsources})