SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(DiscardServer cetty  "DiscardServerHandler.cpp")
ADD_DEPENDENCIES(DiscardServer cetty)
