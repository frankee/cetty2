# Static versions of Cetty libraries.  We build them using more
# strict warnings than what are used for other targets, to ensure that
# cetty can be compiled by a user aggressive about warnings.

AUX_SOURCE_DIRECTORY(cetty/gearman GEARMAN_DIR)
AUX_SOURCE_DIRECTORY(cetty/gearman/builder GEARMAN_BUILDER_DIR)
AUX_SOURCE_DIRECTORY(cetty/gearman/protobuf GEARMAN_PROTOBUF_DIR)
AUX_SOURCE_DIRECTORY(cetty/gearman/protobuf/builder GEARMAN_PROTOBUF_BUILDER_DIR)

SET(gearman_sources ${GEARMAN_DIR} ${GEARMAN_BUILDER_DIR} ${GEARMAN_PROTOBUF_DIR}
${GEARMAN_PROTOBUF_BUILDER_DIR})

cxx_static_library(cettygearman ${gearman_sources})
