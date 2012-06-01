SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(TelnetClient cetty "TelnetClientHandler.cpp")
ADD_DEPENDENCIES(TelnetClient cetty)