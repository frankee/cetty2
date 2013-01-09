#find geos

set(GEOS_HEADER_FILES geos.h)

FIND_PATH(GEOS_INCLUDE_DIR ${GEOS_HEADER_FILES}
	/usr/include
        /usr/local/include
        /opt/include
        /opt/geos/include)

mark_as_advanced(GEOS_INCLUDE_DIR)

FIND_LIBRARY(GEOS_LIBRARY_DIR 
	NAMES geos
	PATHS /usr/local/lib
              /usr/local/lib64
              /opt/lib
              /opt/lib64
              /opt/geos/lib
              /opt/geos/lib64)

mark_as_advanced(GEOS_LIBRARY_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOS DEFAULT_MSG
    GEOS_LIBRARY_DIR GEOS_INCLUDE_DIR)

SET(GEOS_INCLUDE_DIRS ${GEOS_INCLUDE_DIR})
SET(GEOS_LIBRARY_DIRS ${GEOS_LIBRARY_DIR})
