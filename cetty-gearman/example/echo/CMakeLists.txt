SET(DEPENDENCIES cetty-gearman cetty-service cetty-config cetty yaml-cpp)

cxx_executable_current_path_no_link(GearmanEchoWorker)
cxx_link(GearmanEchoWorker ${DEPENDENCIES} rt)

cxx_executable_current_path_no_link(GearmanEchoClient)
cxx_link(GearmanEchoClient ${DEPENDENCIES} rt)

ADD_SUBDIRECTORY(protobuf)
