#find LOG4CPLUS

SET(GPERFTOOLS_HEADER_FILE gperftools/tcmalloc.h)

FIND_PATH(GPERFTOOLS_INCLUDE_DIR ${GPERFTOOLS_HEADER_FILE}
	/usr/local/include
    /usr/include
	/opt/gperftools/include
  )
mark_as_advanced(GPERFTOOLS_INCLUDE_DIR)

FIND_LIBRARY(GPERFTOOLS_LIBRARY_DIR NAMES tcmalloc_minimal PATHS
	/usr/local/lib
    /usr/local/lib64
	/usr/lib
    /usr/lib64
	/opt/gperftools/lib
    /opt/gperftools/lib64
  )
mark_as_advanced(GPERFTOOLS_LIBRARY_DIR)

SET(GPERFTOOLS_INCLUDE_DIRS ${GPERFTOOLS_INCLUDE_DIR})
SET(GPERFTOOLS_LIBRARY_DIRS ${GPERFTOOLS_LIBRARY_DIR})