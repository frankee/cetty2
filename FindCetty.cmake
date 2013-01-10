#find cetty
#SET(CETTY_HEADER_FILE cetty/bootstrap/Bootstrap.h)

set(CETTY_HEADER_FILES cetty/Version.h)

FIND_PATH(CETTY_INCLUDE_DIR ${CETTY_HEADER_FILES}
	/usr/include
    /usr/local/include
    /opt/include
    /opt/cetty/include)

mark_as_advanced(CETTY_INCLUDE_DIR)

FIND_LIBRARY(CETTY_LIBRARY_DIR 
	NAMES cetty
	PATHS /usr/local/lib
          /usr/local/lib64
          /opt/lib
          /opt/lib64
          /opt/cetty/lib
          /opt/cetty/lib64)
		  
FIND_PROGRAM(CETTY_SERVICE_GENERATOR ProtobufServiceGenerator
    /usr/bin
	/usr/local/bin
	/opt/bin
	/opt/cetty/bin)
	
FIND_PROGRAM(CETTY_CONFIG_GENERATOR ConfigGenerator
    /usr/bin
	/usr/local/bin
	/opt/bin
	/opt/cetty/bin)
	
if (CETTY_INCLUDE_DIR AND CETTY_LIBRARY_DIR)
    set(CETTY_FOUND TRUE)	
endif (CETTY_INCLUDE_DIR AND CETTY_LIBRARY_DIR)

if (CETTY_FOUND)
    message(STATUS "found cetty: ${CETTY_LIBRARY}")
else (CETTY_FOUND)
    message(STATUS "could not find cetty library")
endif (CETTY_FOUND)

mark_as_advanced(CETTY_LIBRARY_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CETTY DEFAULT_MSG
    CETTY_LIBRARY_DIR CETTY_INCLUDE_DIR)

SET(CETTY_INCLUDE_DIRS ${CETTY_INCLUDE_DIR})
SET(CETTY_LIBRARY_DIRS ${CETTY_LIBRARY_DIR})

