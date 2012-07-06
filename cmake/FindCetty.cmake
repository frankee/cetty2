#find cetty
#SET(CETTY_HEADER_FILE cetty/bootstrap/Bootstrap.h)

FIND_PATH(CETTY_INCLUDE_DIR cetty.h
	/mnt/hgfs/alp/Cetty/include
	/mnt/hgfs/alp-2/Cetty/include
	/usr/local/include
    /usr/include
  )
mark_as_advanced(CETTY_INCLUDE_DIR)

FIND_LIBRARY(CETTY_LIBRARY_DIR 
	NAMES cetty
	PATHS /mnt/hgfs/alp/Cetty/buildcmake/lib/
		  /mnt/hgfs/alp-2/Cetty/buildcmake/lib/
	#PATHS /mnt/hgfs/alp/Cetty/buildfpic/lib/
		  /usr/local/lib
	#HINTS /mnt/hgfs/alp/Cetty/buildcmake/lib/
	#PATH_SUFFIXES /mnt/hgfs/alp/Cetty/buildcmake/lib/
  )
mark_as_advanced(CETTY_LIBRARY_DIR)

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CETTY DEFAULT_MSG
    CETTY_LIBRARY_DIR CETTY_INCLUDE_DIR)
  
SET(CETTY_INCLUDE_DIRS ${CETTY_INCLUDE_DIR})
SET(CETTY_LIBRARY_DIRS ${CETTY_LIBRARY_DIR})
