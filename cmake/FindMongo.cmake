#find LOG4CPLUS

SET(MONGO_HEADER_FILE mongo/client/dbclient.h)

FIND_PATH(MONGO_INCLUDE_DIR ${MONGO_HEADER_FILE}
	/usr/local/include
    /usr/include
	/opt/mongo/include
  )

FIND_LIBRARY(MONGO_LIBRARY
 NAMES mongoclient
 PATH_SUFFIXES lib64 lib
 PATHS
	/usr/local
	/usr
	/opt/mongo)
	
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MONGO DEFAULT_MSG
    MONGO_LIBRARY MONGO_INCLUDE_DIR)

if (MONGO_FOUND)
    message(STATUS "found mongo: ${MONGO_LIBRARY} ${MONGO_INCLUDE_DIR}")
endif (MONGO_FOUND)
	
mark_as_advanced(MONGO_LIBRARY MONGO_INCLUDE_DIR)

SET(MONGO_INCLUDE_DIRS ${MONGO_INCLUDE_DIR})
SET(MONGO_LIBRARYS ${MONGO_LIBRARY})