/*
 * SessionManagerConfig.cpp
 *
 *  Created on: 2012-10-9
 *      Author: chenhl
 */
#include <cetty/shiro/session/SessionManagerConfig.h>
#include <cetty/shiro/session/SessionManager.h>

#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace session {

CETTY_CONFIG_ADD_DESCRIPTOR(SessionManagerConfig,
                            new ConfigDescriptor(
                                4,
                                CETTY_CONFIG_FIELD(SessionManagerConfig, deleteInvalidSessions, BOOL),
                                CETTY_CONFIG_FIELD(SessionManagerConfig, sessionValidationSchedulerEnabled, BOOL),
                                CETTY_CONFIG_FIELD(SessionManagerConfig, globalSessionTimeout, INT32),
                                CETTY_CONFIG_FIELD(SessionManagerConfig, sessionValidationInterval, INT32)
                            ),
                            new SessionManagerConfig);

SessionManagerConfig::SessionManagerConfig()
    : ConfigObject("SessionManagerConfig"),
      deleteInvalidSessions(true),
      sessionValidationSchedulerEnabled(false),
      globalSessionTimeout(SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT),
      sessionValidationInterval(SessionManager::DEFAULT_SESSION_VALIDATION_INTERVAL){}

}
}
}



