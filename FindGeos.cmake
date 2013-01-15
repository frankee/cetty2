#find geos

set(GEOS_HEADER_FILES geos.h)

FIND_PATH(GEOS_INCLUDE_DIR ${GEOS_HEADER_FILES}
	/usr/include
        /usr/local/include
        /opt/include
        /opt/geos/include)

FIND_LIBRARY(GEOS_LIBRARY 
	NAMES geos
	PATH_SUFFIXES lib64 lib
	PATHS /usr/local
              /opt
              /opt/geos)



include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOS DEFAULT_MSG
    GEOS_LIBRARY GEOS_INCLUDE_DIR)

if (GEOS_FOUND)
    message(STATUS "found geos: ${GEOS_LIBRARY}")
else (GEOS_FOUND)
    message(STATUS "could not find geos library")
endif (GEOS_FOUND)

mark_as_advanced(GEOS_LIBRARY GEOS_INCLUDE_DIR)

SET(GEOS_INCLUDE_DIRS ${GEOS_INCLUDE_DIR})
SET(GEOS_LIBRARYS ${GEOS_LIBRARY})
