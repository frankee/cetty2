SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(server  "server.cpp" "echo.pb.cc")
set_target_properties(server PROPERTIES COMPILE_FLAGS "${cxx_flags}")
target_link_libraries(server cetty cettyconfig cettyservice protobufservcie)

add_executable(client  "client.cpp" "echo.pb.cc")
set_target_properties(client PROPERTIES COMPILE_FLAGS "${cxx_flags}")
target_link_libraries(client cetty cettyconfig cettyservice protobufservcie)

#cxx_executable_current_path(client cetty cettyconfig cettyservice protobufservcie "echo.pb.cc")
#ADD_DEPENDENCIES(client cetty cettyconfig cettyservice protobufservcie)
