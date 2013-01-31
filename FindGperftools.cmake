#find LOG4CPLUS

SET(GPERFTOOLS_HEADER_FILE gperftools/tcmalloc.h)

FIND_PATH(GPERFTOOLS_INCLUDE_DIR ${GPERFTOOLS_HEADER_FILE}
	/usr/local/include
    /usr/include
	/opt/gperftools/include)
mark_as_advanced(GPERFTOOLS_INCLUDE_DIR)

FIND_LIBRARY(GPERFTOOLS_LIBRARY
   NAMES tcmalloc_minimal
   PATH_SUFFIXES lib64 lib
   PATHS
	/usr/local
	/usr
	/opt/gperftools)
	
GET_FILENAME_COMPONENT(GPERFTOOLS_LIBRARY_DIR ${GPERFTOOLS_LIBRARY} PATH)
  
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GPERFTOOLS DEFAULT_MSG
    GPERFTOOLS_LIBRARY GPERFTOOLS_INCLUDE_DIR)

if (GPERFTOOLS_FOUND)
    message(STATUS "found GPERFTOOLS: ${GPERFTOOLS_LIBRARY}")
endif (GPERFTOOLS_FOUND)

mark_as_advanced(GPERFTOOLS_LIBRARY)

SET(GPERFTOOLS_INCLUDE_DIRS ${GPERFTOOLS_INCLUDE_DIR})
SET(GPERFTOOLS_LIBRARYS ${GPERFTOOLS_LIBRARY})