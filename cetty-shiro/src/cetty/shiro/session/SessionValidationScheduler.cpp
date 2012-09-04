/*
 * SessionValidationScheduler.cpp
 *
 *  Created on: 2012-8-15
 *      Author: chenhl
 */

#include <cetty/shiro/session/SessionValidationScheduler.h>
#include <cetty/shiro/session/SessionManager.h>


namespace cetty {
namespace shiro {
namespace session {
SessionValidationScheduler::SessionValidationScheduler()
  :sessionManager(NULL),
   enabled(false),
   interval(SessionManager::DEFAULT_SESSION_VALIDATION_INTERVAL) {}

SessionValidationScheduler::SessionValidationScheduler(SessionManager *sessionManager)
   :enabled(false),
   interval(SessionManager::DEFAULT_SESSION_VALIDATION_INTERVAL) {
   this->sessionManager = sessionManager;
    }
}
}
}

