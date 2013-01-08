set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.48.0 COMPONENTS date_time system thread filesystem regex program_options REQUIRED )

INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
LINK_DIRECTORIES(${BOOST_LIB_DIRS})


# Defines CMAKE_USE_PTHREADS_INIT and CMAKE_THREAD_LIBS_INIT.
FIND_PACKAGE(Threads)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
LINK_DIRECTORIES(${PROTOBUF_LIB_DIRS})

find_package(YamlCpp REQUIRED)
include_directories(${YAMLCPP_INCLUDE_DIRS})
LINK_DIRECTORIES(${YAMLCPP_LIBRARY_DIRS})

find_package(Soci)
include_directories(${SOCI_INCLUDE_DIRS})
LINK_DIRECTORIES(${SOCI_LIBRARY_DIR})

find_package(Gperftools)
if (Gperftools_found)
    include_directories(${GPERFTOOLS_INCLUDE_DIRS})
    LINK_DIRECTORIES(${GPERFTOOLS_LIBRARY_DIRS})
else (Gperftools_found)
    message(STATUS "Gperftools library not found")
endif (Gperftools_found)

find_package(Mongo)
if (Mongo_found)
    include_directories(${MONGO_INCLUDE_DIR})
    LINK_DIRECTORIES(${MONGO_LIBRARY_DIRS})
else (Mongo_found)
    message(STATUS "mongo library not found")
endif (Mongo_found)
