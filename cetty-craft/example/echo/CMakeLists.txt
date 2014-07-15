#

GENERATE_SERVICE(PROTO_SRCS echo.proto)
message(STATUS "craft echo example using ${PROTO_SRCS}")

cxx_executable_current_path_no_link(CraftEchoServer ${PROTO_SRCS})
cxx_link_pb(CraftEchoServer ${CRAFT_PROJECT_DEPENDENCIES} rt)
