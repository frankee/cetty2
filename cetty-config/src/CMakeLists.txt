# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/config CONFIG_DIR)

SET(libsources ${CONFIG_DIR})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
cxx_static_library(cettyconfig "${cxx_default}" ${libsources})
