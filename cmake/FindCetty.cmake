#find cetty
#SET(CETTY_HEADER_FILE cetty/bootstrap/Bootstrap.h)

set(CETTY_HEADER_FILES cetty/bootstrap/Bootstrap.h)

FIND_PATH(CETTY_INCLUDE_DIR ${CETTY_HEADER_FILES}
	/usr/include
        /usr/local/include
        /opt/include
        /opt/cetty/include
	/mnt/s/tt
	/mnt/hgfs/alp-2/
	/usr/local/include
    /usr/include
  )
mark_as_advanced(CETTY_INCLUDE_DIR)

FIND_LIBRARY(CETTY_LIBRARY_DIR 
	NAMES cetty
	PATHS /mnt/hgfs/alp_new/cetty/mybuild/lib
		  /mnt/hgfs/alp/Cetty/buildcmake/lib/
		  /mnt/hgfs/alp-2/Cetty/buildcmake/lib/
	#PATHS /mnt/hgfs/alp/Cetty/buildfpic/lib/
		  /usr/local/lib
	#HINTS /mnt/hgfs/alp/Cetty/buildcmake/lib/
	#PATH_SUFFIXES /mnt/hgfs/alp/Cetty/buildcmake/lib/
  )
mark_as_advanced(CETTY_LIBRARY_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CETTY DEFAULT_MSG
    CETTY_LIBRARY_DIR CETTY_INCLUDE_DIR)
  
SET(CETTY_INCLUDE_DIRS ${CETTY_INCLUDE_DIR})
SET(CETTY_LIBRARY_DIRS ${CETTY_LIBRARY_DIR})
