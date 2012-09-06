/*
 * DelegateSession.h
 *
 *  Created on: 2012-9-6
 *      Author: chenhl
 */

#if !defined(CETTY_SHIRO_SESSION_DELEGATESESSION_H)
#define CETTY_SHIRO_SESSION_DELEGATESESSION_H

#include <cetty/shiro/session/SessionManager.h>
#include <cetty/shiro/session/SessionPtr.h>

namespace cetty {
namespace shiro {
namespace session {

class DelegateSession {
public:
    DelegateSession():manager(NULL){}
    DelegateSession(const std::string &id, SessionManager *manager)
        :id(id), manager(manager){}

    void setId(const std::string &id){
        this->id = id;
    }
    const std::string &getId() const { return this->id; }

    SessionManager *getSessionManager() const{ return manager; }
    void setSessionManager(SessionManager *manager){
        this->manager = manager;
    }

    SessionPtr getSession(){
        if(session == NULL)
            session = manager->getSession(id);
        return session;
    }

    void setAttribute(const std::string& key, const std::string& value){
        this->manager->setAttribute(id, key, value);
    }

    std::string &getAttribute(const std::string &key){
        return this->manager->getAttribute(id, key);
    }

    void removeAttribute(const std::string& key){
        this->manager->removeAttribute(id, key);
    }

    bool isLogin(){ return this->manager->isLogin(id); }
    void setLogin(bool login){ this->manager->setLogin(id, login); }

    void touch(){ this->manager->touch(id); }

    void stop(){ this->manager->stop(id); }

private:
    std::string id;
    SessionManager *manager;
    SessionPtr session;
};

}
}
}


#endif // CETTY_SHIRO_SESSION_DELEGATESESSION_H
