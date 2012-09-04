/*
 * PrincipalCollection.cpp
 *
 *  Created on: 2012-8-16
 *      Author: chenhl
 */
#include <cetty/shiro/subject/PrincipalCollection.h>
#include <cetty/shiro/util/EmptyObj.h>

namespace cetty {
namespace shiro {
namespace subject {

using namespace cetty::shiro::util;

void PrincipalCollection::add(const std::string &principal, const std::string &realmName){
    if (realmName.empty() || principal.empty()) return;

    /** This process will product a couple of temp object. non efficient */
    this->cachedToString.clear();
    std::map<std::string, std::vector<std::string>>::iterator it = realmPrincipals.find(realmName);
    if(it == realmPrincipals.end()){
        std::vector<std::string> tmp;
        tmp.push_back(principal);
        realmPrincipals.insert(std::pair<std::string, std::vector<std::string>>(realmName, tmp));
    }
    else{
        it->second.push_back(principal);
    }
}

void PrincipalCollection::addAll(const std::vector<std::string> &principals, const std::string &realmName){
    if(realmName.empty() || principals.empty()) return;
    this->cachedToString.clear();
    std::map<std::string, std::vector<std::string>>::iterator it = realmPrincipals.find(realmName);
    if(it == realmPrincipals.end())
        realmPrincipals.insert(std::pair<std::string, std::vector<std::string>>(realmName, principals));
    else{
        std::vector<std::string>::const_iterator begin  = principals.begin();
        for(; begin != principals.end(); ++begin)
            it->second.push_back(*begin);
    }
}

void PrincipalCollection::addAll(const PrincipalCollection &principals){
    if(principals.isEmpty()) return;
    std::vector<std::string> realmNames;
    principals.getRealmNames(&realmNames);
    if(realmNames.empty()) return;

    std::vector<std::string>::iterator it = realmNames.begin();
    for(; it != realmNames.end(); ++it) {
        std::vector<std::string> tmp;
        principals.fromRealm(*it, &tmp);
        if(tmp.empty()) continue;
        addAll(tmp, *it);
    }
}

std::string PrincipalCollection::getPrimaryPrincipal() const{
    std::vector<std::string> principals;
    asVector(&principals);
    if(principals.empty()) return emptyString;
    return principals.at(0);
}

void PrincipalCollection::asVector(std::vector<std::string> *principals) const{
    if(principals == NULL) return;
    std::map<std::string, std::vector<std::string>>::const_iterator it = realmPrincipals.begin();
    for(; it != realmPrincipals.end(); ++it){
        if((it->first).empty() || (it->second).empty()) continue;
        std::vector<std::string>::const_iterator begin = (it->second).begin();
        for(; begin != (it->second).end(); ++begin)
            principals->push_back(*begin);
    }
}

void PrincipalCollection::fromRealm(const std::string &realmName, std::vector<std::string> *principals) const{
    if(realmName.empty() || principals == NULL) return;
    std::map<std::string, std::vector<std::string>>::const_iterator it = realmPrincipals.find(realmName);
    if(it == realmPrincipals.end()) return;
    *principals = it->second;
}

void PrincipalCollection::getRealmNames(std::vector<std::string> *realmNames) const{
    if(realmNames == NULL) return;
    std::map<std::string, std::vector<std::string>>::const_iterator it = realmPrincipals.begin();
    for(; it != realmPrincipals.end(); ++it){
        if((it->first).empty()) continue;
        realmNames->push_back(it->first);
    }
}

bool PrincipalCollection::isEmpty() const{
    return realmPrincipals.empty();
}

void PrincipalCollection::clear(){
    realmPrincipals.clear();
}

}
}
}




