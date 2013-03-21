# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/beanstalk BEANSTALK_DIR)
AUX_SOURCE_DIRECTORY(cetty/beanstalk/protocol BEANSTALK_PROTOCOL_DIR)
AUX_SOURCE_DIRECTORY(cetty/beanstalk/protocol/commands BEANSTALK_PROTOCOL_COMMANDS_DIR)

SET(beanstalk_sources ${BEANSTALK_DIR} ${BEANSTALK_PROTOCOL_DIR} ${BEANSTALK_PROTOCOL_COMMANDS_DIR})
				  
SET(beanstalk_lib cetty-beanstalk)

cxx_static_library(${beanstalk_lib} ${beanstalk_sources})

install(TARGETS ${beanstalk_lib} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/beanstalk/ DESTINATION include/cetty/beanstalk)
