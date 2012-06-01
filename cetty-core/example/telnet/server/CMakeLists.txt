SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(TelnetServer cetty "TelnetServerHandler.cpp")
ADD_DEPENDENCIES(TelnetServer cetty)