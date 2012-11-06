# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/redis REDIS_DIR)
AUX_SOURCE_DIRECTORY(cetty/redis/builder REDIS_BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/redis/protocol REDIS_PROTOCOL_DIR)
AUX_SOURCE_DIRECTORY(cetty/redis/protocol/commands REDIS_PROTOCOL_COMMANDS_DIR)

SET(redis_sources ${REDIS_DIR} ${REDIS_BUILDER_DIR}
                  ${REDIS_PROTOCOL_DIR} ${REDIS_PROTOCOL_COMMANDS_DIR})
				  
SET(redis_lib cetty-redis)

cxx_static_library(${redis_lib} ${redis_sources})

install(TARGETS ${redis_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/redis/ DESTINATION include/cetty/redis)
