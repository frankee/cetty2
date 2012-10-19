# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/shiro SHIRO_DIR)

AUX_SOURCE_DIRECTORY(cetty/shiro/authc SHIRO_AUTHC_DIR)
AUX_SOURCE_DIRECTORY(cetty/shiro/authz SHIRO_AUTHZ_DIR)
AUX_SOURCE_DIRECTORY(cetty/shiro/realm SHIRO_REALM_DIR)
AUX_SOURCE_DIRECTORY(cetty/shiro/session SHIRO_SESSION_DIR)
AUX_SOURCE_DIRECTORY(cetty/shiro/util SHIRO_UTIL_DIR)

SET(shiro_sources ${SHIRO_DIR} ${SHIRO_AUTHC_DIR} ${SHIRO_AUTHZ_DIR} 
  ${SHIRO_REALM_DIR} ${SHIRO_SESSION_DIR} ${SHIRO_UTIL_DIR})
SET(shiro_lib cetty-shiro)

cxx_static_library(${shiro_lib} ${shiro_sources})

install(TARGETS ${shiro_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/shiro/ DESTINATION include/cetty/shiro)
