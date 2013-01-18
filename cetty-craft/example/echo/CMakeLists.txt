#

GENERATE_SERVICE(PROTO_SRCS echo.proto)
message(STATUS "craft echo example using ${PROTO_SRCS}")

cxx_executable_current_path_no_link(CraftEchoServer ${PROTO_SRCS})

if (GPERFTOOLS_LIBRARY)
    cxx_link_with_pb(CraftEchoServer ${GPERFTOOLS_LIBRARY} ${CRAFT_PROJECT_DEPENDENCIES})
else (GPERFTOOLS_LIBRARY)
    cxx_link_with_pb(CraftEchoServer ${CRAFT_PROJECT_DEPENDENCIES})
endif (GPERFTOOLS_LIBRARY)
