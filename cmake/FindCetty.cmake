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

mark_as_advanced(CETTY_LIBRARY_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CETTY DEFAULT_MSG
    CETTY_LIBRARY_DIR CETTY_INCLUDE_DIR)

SET(CETTY_INCLUDE_DIRS ${CETTY_INCLUDE_DIR})
SET(CETTY_LIBRARY_DIRS ${CETTY_LIBRARY_DIR})
