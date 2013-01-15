#find cetty
#SET(ALP_HEADER_FILE cetty/bootstrap/Bootstrap.h)

set(ALP_HEADER_FILES alp/core/Util.h)

FIND_PATH(ALP_INCLUDE_DIR ${ALP_HEADER_FILES}
	/usr/include
        /usr/local/include
        /opt/include
        /opt/alp/include
		/home/projects/alp/include)

FIND_LIBRARY(ALP_LIBRARY
	NAMES alp
	PATH_SUFFIXES lib64 lib
	PATHS /usr/local
          /opt
          /opt/alp
		  /home/projects/alp)


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALP DEFAULT_MSG
    ALP_LIBRARY ALP_INCLUDE_DIR)

if (ALP_FOUND)
    message(STATUS "found alp: ${ALP_LIBRARY}")
else (ALP_FOUND)
    message(STATUS "could not find alp library")
endif (ALP_FOUND)

mark_as_advanced(ALP_INCLUDE_DIR ALP_LIBRARY)
	
SET(ALP_INCLUDE_DIRS ${ALP_INCLUDE_DIR})
SET(ALP_LIBRARYS ${ALP_LIBRARY})
