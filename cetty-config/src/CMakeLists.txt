# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/config CONFIG_DIR)

SET(configure_sources ${CONFIG_DIR})
SET(configure_lib cetty-config)

cxx_static_library(${configure_lib} ${configure_sources})

install(TARGETS ${configure_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/config/
  DESTINATION include/cetty/config)