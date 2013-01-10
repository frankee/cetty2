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

find_package(Cetty REQUIRED)
if (CETTY_FOUND)
    include_directories(${CETTY_INCLUDE_DIR})
    LINK_DIRECTORIES(${CETTY_LIBRARY_DIR})
else (CETTY_FOUND)
    message(STATUS "cetty library not found")
endif (CETTY_FOUND)

find_package(GDAL)
if (GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    LINK_DIRECTORIES(${GDAL_LIBRARY})
else (GDAL_FOUND)
    message(STATUS "GDAL library not found")
endif(GDAL_FOUND)

find_package(Geos)
if (Geos_FOUND)
    include_directories(${GEOS_INCLUDE_DIR})
    LINK_DIRECTORIES(${GEOS_LIBRARY_DIR})
else (Geos_FOUND)
    message(STATUS "Geos library not found")
endif (Geos_FOUND)
