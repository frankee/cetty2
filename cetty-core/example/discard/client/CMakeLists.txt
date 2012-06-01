SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
cxx_executable_current_path(DiscardClient cetty  "DiscardClientHandler.cpp")
ADD_DEPENDENCIES(DiscardClient cetty)