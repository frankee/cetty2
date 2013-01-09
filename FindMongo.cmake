#find LOG4CPLUS

SET(MONGO_HEADER_FILE mongo/client/dbclient.h)

FIND_PATH(MONGO_INCLUDE_DIR ${MONGO_HEADER_FILE}
	/usr/local/include
    /usr/include
	/opt/mongo/include
  )
mark_as_advanced(MONGO_INCLUDE_DIR)

FIND_LIBRARY(MONGO_LIBRARY_DIR NAMES mongoclient PATHS
	/usr/local/lib
    /usr/local/lib64
	/usr/lib
    /usr/lib64
	/opt/mongo/lib
    /opt/mongo/lib64
  )
mark_as_advanced(MONGO_LIBRARY_DIR)

SET(MONGO_INCLUDE_DIRS ${MONGO_INCLUDE_DIR})
SET(MONGO_LIBRARY_DIRS ${MONGO_LIBRARY_DIR})