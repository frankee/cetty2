# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/service/builder BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/service/pool POOL_DIR)
AUX_SOURCE_DIRECTORY(cetty/service/http HTTP_DIR)
AUX_SOURCE_DIRECTORY(cetty/service SERVICE_DIR)

SET(service_sources ${BUILDER_DIR} ${HTTP_DIR} ${POOL_DIR} ${SERVICE_DIR})
SET(service_lib cetty-service)

cxx_static_library(${service_lib} ${service_sources})

install(TARGETS ${service_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/service/ DESTINATION include/cetty/service)
