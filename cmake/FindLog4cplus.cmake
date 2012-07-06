#find LOG4CPLUS

SET(LOG4CPLUS_HEADER_FILE
 config/defines.hxx
 config/macosx.h
 config/win32.h
 helpers/appenderattachableimpl.h
 helpers/loglog.h
 helpers/logloguser.h
 helpers/pointer.h
 helpers/property.h
 helpers/sleep.h
 helpers/socket.h
 helpers/socketbuffer.h
 helpers/stringhelper.h
 helpers/syncprims-pthreads.h
 helpers/syncprims-win32.h
 helpers/syncprims.h
 helpers/thread-config.h
 helpers/threads.h
 helpers/timehelper.h
 spi/appenderattachable.h
 spi/factory.h
 spi/filter.h
 spi/loggerfactory.h
 spi/loggerimpl.h
 spi/loggingevent.h
 spi/objectregistry.h
 spi/rootlogger.h
 appender.h
 config.h
 config.hxx
 configurator.h
 consoleappender.h
 fileappender.h
 fstreams.h
 hierarchy.h
 hierarchylocker.h
 layout.h
 logger.h
 loggingmacros.h
 loglevel.h
 ndc.h
 nteventlogappender.h
 nullappender.h
 socketappender.h
 streams.h
 syslogappender.h
 tstring.h
 version.h
 win32consoleappender.h
 win32debugappender.h)

FIND_PATH(LOG4CPLUS_INCLUDE_DIR ${LOG4CPLUS_HEADER_FILE}
	/usr/local/include/log4cplus
    /usr/include/log4cplus
  )
mark_as_advanced(LOG4CPLUS_INCLUDE_DIR)

FIND_LIBRARY(LOG4CPLUS_LIBRARY_DIR NAMES log4cplus PATHS
	/usr/local/lib 
	/usr/lib
  )
mark_as_advanced(LOG4CPLUS_LIBRARY_DIR)

if(LOG4CPLUS_INCLUDE_DIR)
add_definitions(-DHAS_LOG4_CPLUS)
endif()
  
SET(LOG4CPLUS_INCLUDE_DIRS ${LOG4CPLUS_INCLUDE_DIR})
SET(LOG4CPLUS_LIBRARY_DIRS ${LOG4CPLUS_LIBRARY_DIR})