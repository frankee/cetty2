/*
 * SessionContext.cpp
 *
 *  Created on: 2012-8-15
 *      Author: chenhl
 */

#include <cetty/shiro/session/SessionContext.h>

namespace cetty {
namespace shiro {
namespace session {

const std::string SessionContext::HOST = "cetty.shiro.session.sessionContext.host";
const std::string SessionContext::SESSION_ID = "cetty.shiro.session.sessionContext.sessionId";

void SessionContext::putAll(std::map<std::string, std::string> &context){
    this->context.insert(context.begin(), context.end());
}

const std::string SessionContext::get (const std::string &key) {
    std::map<std::string, std::string>::iterator it = context.find(key);
    if(it == context.end()) return std::string();
    return it->second;
}

void SessionContext::put(const std::string &key, const std::string &value){
    if(!key.empty()){
        context.insert(std::pair<std::string, std::string>(key, value));
    }
}

}
}
}


