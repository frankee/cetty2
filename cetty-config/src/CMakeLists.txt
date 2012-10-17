# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/config CONFIG_DIR)

SET(configure_sources ${CONFIG_DIR})

cxx_static_library(cettyconfig ${configure_sources})
