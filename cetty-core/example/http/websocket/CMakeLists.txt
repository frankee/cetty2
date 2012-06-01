SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(WebSocketServer cetty "WebSocketServerIndexPage.cpp"  "WebSocketServerHandler.cpp")
ADD_DEPENDENCIES(WebSocketServer cetty)