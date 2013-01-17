#find cetty
#SET(CETTY_HEADER_FILE cetty/bootstrap/Bootstrap.h)

set(CETTY_HEADER_FILES cetty/Version.h)

FIND_PATH(CETTY_INCLUDE_DIR ${CETTY_HEADER_FILES}
	/usr/include
    /usr/local/include
    /opt/include
    /opt/cetty/include
	/home/projects/cetty/include)

FIND_LIBRARY(CETTY_LIBRARY
    NAMES cetty cetty-config cetty-craft cetty-gearman cetty-protobuf cetty-protobuf-serialization cetty-redis cetty-service
	PATH_SUFFIXES lib64 lib
	PATHS /usr/local
          /opt
          /opt/cetty
          /home/projects/cetty
		  /home/projects/cetty/builder)
		  
GET_FILENAME_COMPONENT(CETTY_LIBRARY_DIR ${CETTY_LIBRARY} PATH)

FIND_PROGRAM(CETTY_SERVICE_GENERATOR ProtobufServiceGenerator
    /usr/bin
	/usr/local/bin
	/opt/bin
	/opt/cetty/bin
	/home/projects/cetty/bin
	/home/projects/cetty/builder/bin)
	
FIND_PROGRAM(CETTY_CONFIG_GENERATOR ConfigGenerator
    /usr/bin
	/usr/local/bin
	/opt/bin
	/opt/cetty/bin
	/home/projects/cetty/bin
	/home/projects/cetty/builder/bin)
	
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CETTY DEFAULT_MSG
    CETTY_LIBRARY CETTY_INCLUDE_DIR)

if (CETTY_FOUND)
    message(STATUS "found cetty: ${CETTY_LIBRARY} ${CETTY_INCLUDE_DIR}")
else (CETTY_FOUND)
    message(STATUS "could not find cetty library")
endif (CETTY_FOUND)

mark_as_advanced(CETTY_LIBRARY CETTY_INCLUDE_DIR)

SET(CETTY_INCLUDE_DIRS ${CETTY_INCLUDE_DIR})
SET(CETTY_LIBRARYS ${CETTY_LIBRARY})
