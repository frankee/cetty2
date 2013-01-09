#find cetty
#SET(ALP_HEADER_FILE cetty/bootstrap/Bootstrap.h)

set(ALP_HEADER_FILES alp/core/Util.h)

FIND_PATH(ALP_INCLUDE_DIR ${ALP_HEADER_FILES}
	/usr/include
        /usr/local/include
        /opt/include
        /opt/alp/include)

mark_as_advanced(ALP_INCLUDE_DIR)

FIND_LIBRARY(ALP_LIBRARY_DIR 
	NAMES alp
	PATHS /usr/local/lib
              /usr/local/lib64
              /opt/lib
              /opt/lib64
              /opt/alp/lib
              /opt/alp/lib64)

mark_as_advanced(ALP_LIBRARY_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALP DEFAULT_MSG
    ALP_LIBRARY_DIR ALP_INCLUDE_DIR)

SET(ALP_INCLUDE_DIRS ${ALP_INCLUDE_DIR})
SET(ALP_LIBRARY_DIRS ${ALP_LIBRARY_DIR})
