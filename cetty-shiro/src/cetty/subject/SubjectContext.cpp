/*
 * SubjectContext.cpp
 *
 *  Created on: 2012-8-10
 *      Author: chenhl
 */

#include <cetty/shiro/subject/SubjectContext.h>
#include <cetty/shiro/util/EmptyObj.h>
#include <cetty/shiro/util/SecurityUtils.h>
#include <cetty/shiro/subject/Subject.h>

namespace cetty {
namespace shiro {
namespace subject {

using namespace cetty::shiro::util;

const std::string SubjectContext::SECURITY_MANAGER = "cetty.shiro.subject.SubjectContext.SECURITY_MANAGER";
const std::string SubjectContext::SESSION_ID =  "cetty.shiro.subject.SubjectContext.SESSION_ID";
const std::string SubjectContext::AUTHENTICATION_TOKEN = "cetty.shiro.subject.SubjectContext.AUTHENTICATION_TOKEN";
const std::string SubjectContext::AUTHENTICATION_INFO = "cetty.shiro.subject.SubjectContext.AUTHENTICATION_INFO";
const std::string SubjectContext::SUBJECT = "cetty.shiro.subject.SubjectContext.SUBJECT";
const std::string SubjectContext::PRINCIPALS = "cetty.shiro.subject.SubjectContext.PRINCIPALS";
const std::string SubjectContext::SESSION = "cetty.shiro.subject.SubjectContext.SESSION";
const std::string SubjectContext::AUTHENTICATED = "cetty.shiro.subject.SubjectContext.AUTHENTICATED";
const std::string SubjectContext::HOST = "cetty.shiro.subject.SubjectContext.HOST";

const std::string SubjectContext::PRINCIPALS_SESSION_KEY = "cetty.shiro.subject.SubjectContext_PRINCIPALS_SESSION_KEY";
const std::string SubjectContext::AUTHENTICATED_SESSION_KEY = "cetty.shiro.subject.SubjectContext_AUTHENTICATED_SESSION_KEY";

boost::any& SubjectContext::get(const std::string &key){
    std::map<std::string, boost::any>::iterator it = context.find(key);
    if(it == context.end()) return emptyAny;
    return it->second;
}

SecurityManager *SubjectContext::getSecurityManager(){
    boost::any any = get(SubjectContext::SECURITY_MANAGER);
    if(any.empty() || any.type() != typeid(SecurityManager *)) return NULL;
    return boost::any_cast<SecurityManager *>(any);
}

void SubjectContext::setSecurityManager(SecurityManager *securityManager){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::SECURITY_MANAGER);
    if(it != context.end()) context.erase(SubjectContext::SECURITY_MANAGER);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::SECURITY_MANAGER, (boost::any)securityManager));
}

SecurityManager *SubjectContext::resolveSecurityManager(){
    SecurityManager *manager = NULL;
    if((manager = getSecurityManager()) == NULL)
        manager = SecurityUtils::getSecurityManager();
    return manager;
}

std::string SubjectContext::getSessionId(){
    boost::any any = get(SubjectContext::SESSION_ID);
    if(any.empty() || any.type() != typeid(std::string)) return emptyString;
    return boost::any_cast<std::string>(any);
}

void SubjectContext::setSessionId(const std::string &sessionId){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::SESSION_ID);
    if(it != context.end()) context.erase(SubjectContext::SESSION_ID);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::SESSION_ID, (boost::any)sessionId));
}

bool SubjectContext::getSubject(Subject *subject){
    if(subject == NULL) return false;
    boost::any any = get(SubjectContext::SUBJECT);
    if(any.empty() || any.type() != typeid(Subject)) return false;
    *subject = boost::any_cast<Subject>(any);
    return true;
}

void SubjectContext::setSubject(const Subject &subject){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::SUBJECT);
    if(it != context.end()) context.erase(SubjectContext::SUBJECT);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::SUBJECT, (boost::any)subject));
}

bool SubjectContext::getPrincipals(PrincipalCollection *principal){
    if(principal == NULL) return false;
    boost::any any = get(SubjectContext::PRINCIPALS);
    if(any.empty() || any.type() != typeid(PrincipalCollection)) return false;
    *principal = boost::any_cast<PrincipalCollection>(any);
    return true;
}

void SubjectContext::setPrincipals(const PrincipalCollection &principals){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::PRINCIPALS);
    if(it != context.end()) context.erase(SubjectContext::PRINCIPALS);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::PRINCIPALS, (boost::any)principals));
}

bool SubjectContext::resolvePrincipals(PrincipalCollection *principal){
    if(getPrincipals(principal)) return true;

    AuthenticationInfo info;
    if(getAuthenticationInfo(&info)){
        if(!(info.getPrincipals().isEmpty()))
        *principal = info.getPrincipals();
        return true;
    }

    Subject subject;
    if(getSubject(&subject)){
        if(!(subject.getPrincipals().isEmpty()))
        *principal = subject.getPrincipals();
        return true;
        }

    SessionPtr session = resolveSession();
    if(session){
        boost::any any = session->getAttribute(SubjectContext::SESSION);
        if(!any.empty() && any.type() == typeid(PrincipalCollection)){
            *principal = boost::any_cast<PrincipalCollection>(any);
            if(!principal->isEmpty()) return true;
        }
    }

    return false;
}

SessionPtr SubjectContext::getSession(){
    boost::any any = get(SubjectContext::SESSION);
    if(any.empty() || any.type() != typeid(SessionPtr)) return SessionPtr();
    return boost::any_cast<SessionPtr>(any);
}

void SubjectContext::setSession(const SessionPtr &session){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::SESSION);
    if(it != context.end()) context.erase(SubjectContext::SESSION);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::SESSION, (boost::any)session));
}

SessionPtr SubjectContext::resolveSession(){
    SessionPtr session = getSession();
    if(session) return session;

    Subject subject;
    if(getSubject(&subject)){
        session = subject.getSession(false);
    }

    return session;
}

bool SubjectContext::isAuthenticated(){
    boost::any any = get(SubjectContext::AUTHENTICATED);
    if(any.empty() || any.type() != typeid(bool)) return false;
    return boost::any_cast<bool>(any);
}

void SubjectContext::setAuthenticated(bool authc){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::AUTHENTICATED);
    if(it != context.end())
        context.erase(SubjectContext::AUTHENTICATED);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::AUTHENTICATED, (boost::any)authc));
}

bool SubjectContext::resolveAuthenticated(){
    bool authc = false;
    boost::any any = get(SubjectContext::AUTHENTICATED);
    if(any.empty()){
        std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::AUTHENTICATION_INFO);
        authc = (it != context.end());
    }else{
        authc = boost::any_cast<bool>(any);
    }
    if(!authc){
        SessionPtr session = resolveSession();
        if(session) {
            boost::any any = session->getAttribute(SubjectContext::AUTHENTICATED_SESSION_KEY);
            if(!any.empty() && any.type() == typeid(bool)) authc = boost::any_cast<bool>(any);
        }
    }

    return authc;

}

bool SubjectContext::getAuthenticationInfo(AuthenticationInfo *info){
    if(info == NULL) return false;

    boost::any any = get(SubjectContext::AUTHENTICATION_INFO);
    if(any.empty() || any.type() != typeid(AuthenticationInfo)) return false;
    *info = boost::any_cast<AuthenticationInfo>(any);
    return true;
}

void SubjectContext::setAuthenticationInfo(const AuthenticationInfo &info){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::AUTHENTICATION_INFO);
    if(it != context.end()) context.erase(SubjectContext::AUTHENTICATION_INFO);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::AUTHENTICATION_INFO, (boost::any)info));
}

bool SubjectContext::getAuthenticationToken(AuthenticationToken *token){
    if(token == NULL) return false;

    boost::any any = get(SubjectContext::AUTHENTICATION_TOKEN);
    if(any.empty() || any.type() != typeid(AuthenticationToken)) return false;
    *token = boost::any_cast<AuthenticationToken>(any);
    return true;
}

void SubjectContext::setAuthenticationToken(const AuthenticationToken &token){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::AUTHENTICATION_TOKEN);
    if(it != context.end()) context.erase(SubjectContext::AUTHENTICATION_TOKEN);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::AUTHENTICATION_TOKEN, (boost::any)token));
}

std::string SubjectContext::SubjectContext::getHost(){
    boost::any any = get(SubjectContext::HOST);
    if(any.empty() || any.type() != typeid(std::string)) return emptyString;
    return boost::any_cast<std::string>(any);
}
void SubjectContext::setHost(std::string host){
    std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::HOST);
    if(it != context.end()) context.erase(SubjectContext::HOST);
    context.insert(std::pair<std::string, boost::any>(SubjectContext::HOST, (boost::any)host));
}
std::string SubjectContext::resolveHost(){
    std::string host = getHost();
    if(host.empty()){
        std::map<std::string, boost::any>::iterator it = context.find(SubjectContext::AUTHENTICATION_TOKEN);
        if(it != context.end()){
            boost::any any= it->second;
            if(!any.empty() && any.type() == typeid(AuthenticationToken))
                host = boost::any_cast<AuthenticationToken>(any).getHost();
        }
    }

    if(host.empty()){
        SessionPtr session = resolveSession();
            if(session) host = session->getHost();
    }

    return host;
}

}
}
}


