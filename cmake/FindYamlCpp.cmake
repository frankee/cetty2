#find LOG4CPLUS

SET(YAMLCPP_HEADER_FILE yaml-cpp/yaml.h)

FIND_PATH(YAMLCPP_INCLUDE_DIR ${YAMLCPP_HEADER_FILE}
	/usr/local/include
    /usr/include
	/opt/yaml/include
  )
mark_as_advanced(YAMLCPP_INCLUDE_DIR)

FIND_LIBRARY(YAMLCPP_LIBRARY_DIR NAMES yaml-cpp PATHS
	/usr/local/lib
	/usr/lib
	/opt/yaml/lib
  )
mark_as_advanced(YAMLCPP_LIBRARY_DIR)

if(YAMLCPP_INCLUDE_DIR)
add_definitions(-DHAS_YAMLCPP)
endif()
  
SET(YAMLCPP_INCLUDE_DIRS ${YAMLCPP_INCLUDE_DIR})
SET(YAMLCPP_LIBRARY_DIRS ${YAMLCPP_LIBRARY_DIR})