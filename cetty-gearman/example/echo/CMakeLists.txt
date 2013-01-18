SET(DEPENDENCIES cetty-gearman cetty-service cetty-config cetty yaml-cpp)

cxx_executable_current_path_no_link(GearmanEchoWorker)

if (GPERFTOOLS_LIBRARY)
    cxx_link(GearmanEchoWorker ${GPERFTOOLS_LIBRARY} ${DEPENDENCIES})
else (GPERFTOOLS_LIBRARY)
    cxx_link(GearmanEchoWorker ${DEPENDENCIES})
endif (GPERFTOOLS_LIBRARY)

cxx_executable_current_path_no_link(GearmanEchoClient)

if (GPERFTOOLS_LIBRARY)
    cxx_link(GearmanEchoClient ${GPERFTOOLS_LIBRARY} ${DEPENDENCIES})
else (GPERFTOOLS_LIBRARY)
    cxx_link(GearmanEchoClient ${DEPENDENCIES})
endif (GPERFTOOLS_LIBRARY)

ADD_SUBDIRECTORY(protobuf)




