# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/config CONFIG_DIR)

AUX_SOURCE_DIRECTORY(cetty/config/generator GENERATOR_DIR)
AUX_SOURCE_DIRECTORY(cetty/config/generator/yaml GENERATOR_YAML_DIR)
AUX_SOURCE_DIRECTORY(cetty/config/generator/cpp GENERATOR_CPP_DIR)

GENERATE_PROTOBUF(PROTO_SRCS PROTO_HDS cetty/config/config_options.proto)

SET(generator_sources ${GENERATOR_DIR}
 ${GENERATOR_YAML_DIR} ${GENERATOR_CPP_DIR} ${PROTO_SRCS})

cxx_executable_no_link(ConfigGenerator cetty/config/generator  ${generator_sources})
cxx_link_pb(ConfigGenerator protoc)

GENERATE_CONFIG(CONFIG_SRCS CONFIG_HDS cetty/config/KeyValuePair.proto)
message(STATUS ${CONFIG_HDS})

SET(configure_sources ${CONFIG_DIR} ${PROTO_SRCS} ${CONFIG_SRCS})
SET(configure_lib cetty-config)

cxx_static_library(${configure_lib} ${configure_sources})

install(TARGETS ${configure_lib} DESTINATION lib)
install(TARGETS ConfigGenerator DESTINATION bin)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/cetty/config/
  DESTINATION include/cetty/config)