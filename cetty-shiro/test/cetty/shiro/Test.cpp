#include <iostream>

#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionPtr.h>
#include <cetty/shiro/session/RedisSessionDAO.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/authc/HashedCredentialsMatcher.h>
#include <cetty/shiro/realm/SqlRealm.h>
#include <cetty/shiro/authz/AuthorizationInfoPtr.h>

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/util/SecurityUtils.h>
#include <cetty/shiro/crypt/SHA1Engine.h>

#include <cetty/config/ConfigCenter.h>
#include <cetty/redis/RedisClient.h>
#include <cetty/redis/builder/RedisClientBuilder.h>
#include <gtest/gtest.h>
#include <cetty/util/StringPiece.h>

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>

using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;
using namespace cetty::shiro::authz;
using namespace cetty::shiro::util;
using namespace cetty::shiro::crypt;
using namespace cetty::shiro::realm;
using namespace cetty::shiro;
using namespace cetty::config;
using namespace cetty::util;
using namespace testing;
using namespace std;

using namespace cetty::redis;
using namespace cetty::redis::builder;


void LoadConfigTest(){
    bool ret = ConfigCenter::instance().loadFromFile("shiro.conf");
    assert(ret);
    std::cout << "///////LoadConfigTest OK///////" << std::endl;
}

TEST(SessionTest, NewSession){
    SessionPtr session = new Session();
    session->setId("001");
    session->setHost("localhost");
    session->setAttribute("key1", "value1");
    session->setAttribute("now", "1137");
    session->setAttribute("", "");
    session->setAttribute("", "nullkey");
    session->setAttribute("nullvale", "");

    std::string sjson;
    session->toJson(&sjson);

    SessionPtr session2 = new Session();
    session2->fromJson(sjson.c_str());
    std::string sjson2;
    session2->toJson(&sjson2);
    EXPECT_STREQ(sjson.c_str(), sjson2.c_str());

    EXPECT_STREQ("value1", session2->getAttribute("key1").c_str());
    EXPECT_STREQ("1137", session2->getAttribute("now").c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////
void createSessionCb(int ret, SessionPtr session){
    EXPECT_EQ(0, ret);
    EXPECT_STREQ("value1", session->getAttribute("key1").c_str());
    EXPECT_STREQ("123", session->getAttribute("key2").c_str());
}

void getSessionCb(int ret, SessionPtr session){
    EXPECT_EQ(0, ret);
    EXPECT_STREQ("value1", session->getAttribute("key1").c_str());
    EXPECT_STREQ("1234", session->getAttribute("key2").c_str());
}

// todo dao.readSession() looks like having problem
TEST(SessionTest, RedisSessionDAO){
    RedisSessionDAO dao;
    SessionPtr session = new Session();
    session->setAttribute("key1", "value1");
    session->setAttribute("key2", "123");
    dao.create(session, boost::bind(createSessionCb, _1, _2));
    dao.readSession(session->getId(), boost::bind(getSessionCb, _1, _2));
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void logout(const std::string &sessionId){
    SecurityManager::instance().logout(sessionId);
}

void SMGetSession(const SessionPtr &session){
    assert(session);
    session->setAttribute("name2", "chenhl");
    session->setAttribute("num2", "2");
    std::cout << session->getAttribute("name") << std::endl;
    std::cout << session->getAttribute("num") << std::endl;
    session->removeAttribute("name");
    session->removeAttribute("num");
    sleep(200);
    logout(const_cast<std::string &>(session->getId()));
}

void SMStartSession(const SessionPtr &session){
    session->setAttribute("sm1", "test1");
    session->setAttribute("sm2", "test2");
    SecurityManager::instance().getSessionManager()->getSession(session->getId(), boost::bind(SMGetSession, _1));
}

void SecurityManagerTest(){
    std::cout << "\n//////////SecurityManagerTest Begin////////////////" << std::endl;
    SecurityManager::instance().getSessionManager()->start("localhost", boost::bind(SMStartSession, _1));

    std::cout << "//////////SecurityManagerTest End////////////////" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void getSession(int ret, const StringPiece &str){
    assert(ret == 0);
    assert(std::string("zhang").compare(str.c_str()));
    std::cout << "//////////RedisGetTest OK////////////////" << std::endl;
}
// todo redis->get() looks like having problem
void RedisTest(){
    std::cout << "\n//////////RedisGetTest Begin////////////////" << std::endl;
    RedisClientBuilder builder;
    builder.addConnection("127.0.0.1", 6379);
    RedisClient *redis = new RedisClient(builder.build());
    redis->get("name", boost::bind(getSession, _1, _2));
    std::cout << "//////////RedisGetTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void WSSETest(){
    std::cout << "\n//////////WSSETest Begin////////////////" << std::endl;
    std::string host = "127.0.0.1";
    std::string principal = "chenhl";
    std::string nonce;
    std::string created;
    WSSE::instance().generatorSecret(principal, host, &nonce, &created);

    bool ret = WSSE::instance().verifySecret(host, principal, nonce, created);
    assert(ret);
    std::cout << "//////////RedisGetTest OK////////////////" << std::endl;
    std::cout << "//////////RedisGetTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void cryptTest(std::string username, std::string origin, int n){
    std::cout << "\n//////////WSSETest Begin////////////////" << std::endl;
    std::string addUsersInfo =
        "INSERT INTO users(username, password) "
        "VALUES (?, ?)";

    SHA1Engine sh1;
    sh1.reset();
    sh1.update(origin);
    DigestEngine::Digest digest = sh1.digest();

    int iterations = n - 1; //already hashed once above
    //iterate remaining number:
    for (int i = 0; i < iterations; i++) {
        sh1.reset();
        sh1.update((const void*)digest.data(), (int)digest.size());
        digest = sh1.digest();
    }

    std::string msg16 = DigestEngine::digestToHex(digest);
    std::cout << msg16 << std::endl;
    std::string oridg;
    DigestEngine::digestFromHex(msg16, &oridg);
    std::cout << oridg << std::endl;

    soci::session sql;

    try {
        sql.open("sqlite3", "/home/chenhl/dev/db/data/sqlite/test");
        sql << addUsersInfo,
               soci::use(username),
               soci::use(msg16);
            //sql.commit();
    } catch (soci::soci_error const& e) {
        LOG_ERROR << e.what();
        //sql.rollback();
    }
    std::cout << "//////////cryptTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void authcCB(const AuthenticationInfoPtr& info){
    assert(info);
    std::cout << info->getPrincipals().getPrimaryPrincipal() << std::endl;
    std::cout << info->getCredentials() << std::endl;
}

void authzCB(const AuthorizationInfoPtr & info){
    assert(info);
    std::vector<std::string> roles = info->getRoles();
    int i = 0;
    for(i = 0; i < roles.size(); ++i)
        std::cout << roles.at(i) << std::endl;
    std::vector<std::string> stringPermissions = info->getStringPermissions();
    for(i = 0; i < stringPermissions.size(); ++i)
        std::cout << stringPermissions.at(i) << std::endl;
}

void sqlRealmTest(){
    std::cout << "\n//////////sqlRealmTest Begin////////////////" << std::endl;
    SqlRealm realm;
    AuthenticationToken token("chenhl", "123456");

    //realm.doGetAuthenticationInfo(token, boost::bind(authcCB, _1));
    realm.doGetAuthorizationInfo( PrincipalCollection(token.getPrincipal(), realm.getName()), boost::bind(authzCB, _1));

    std::cout << "//////////sqlRealmTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void generatorUserToken(std::string &principal, std::string &passwd, AuthenticationToken * token){
    assert(token != NULL);
    std::string host = "127.0.0.1";
    std::string nonce;
    std::string created;
    WSSE::instance().generatorSecret(principal, host, &nonce, &created);

    SHA1Engine sh1;
    sh1.reset();
    sh1.update(passwd);
    DigestEngine::Digest digest = sh1.digest();
    std::string cryptpd1(digest.begin(), digest.end());
    cryptpd1.append(nonce);
    cryptpd1.append(created);
    sh1.reset();
    sh1.update(cryptpd1);
    digest = sh1.digest();
    std::string cryptpd2 = DigestEngine::digestToHex(digest);

    bool ret = WSSE::instance().verifySecret(host, principal, nonce, created);
    token->setPrincipal(principal);
    token->setCredentials(cryptpd2);
    token->setHost(host);

    std::string salt;
    salt.append(nonce);
    salt.append(created);
    token->setSalt(salt);
}

void credentialMatcher(AuthenticationToken &token, const AuthenticationInfoPtr& info){
    HashedCredentialsMatcher matcher;
    assert(matcher.match(token, *info));
    std::cout << "//////////HashedCredentialsMatcherTest OK////////////////" << std::endl;
}

void HashedCredentialsMatcherTest(std::string &principal, std::string &passwd){
    std::cout << "\n//////////HashedCredentialsMatcherTest Begin////////////////" << std::endl;

    AuthenticationToken token;
    generatorUserToken(principal, passwd, &token);

    SqlRealm realm;
    realm.doGetAuthenticationInfo(token, boost::bind(credentialMatcher, token, _1));
    std::cout << "//////////HashedCredentialsMatcherTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void AuthenticatorCB(const AuthenticationInfoPtr& info){
    std::cout << "Authenticator OK" << std::endl;
}

void AuthenticatorTest(std::string principal, std::string passwd){
    std::cout << "\n//////////AuthenticatorTest Begin////////////////" << std::endl;
    AuthenticationToken token;
    generatorUserToken(principal, passwd, &token);

    Authenticator ator;
    ator.authenticate(token, boost::bind(AuthenticatorCB, _1));

    std::cout << "//////////AuthenticatorTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void loginCB(int ret,const AuthenticationToken& token, const AuthenticationInfoPtr& info, const SessionPtr& session){
    assert(ret == 0);
    std::cout << "login OK" << std::endl;
    session->setAttribute("name", "chenhl");
    session->setAttribute("num", "1");
    sleep(15);
    logout(session->getId());

    SecurityManager::instance().getSessionManager()->getSession(session->getId(), boost::bind(SMGetSession, _1));
}

void LoginTest(std::string &principal, std::string &passwd){
    std::cout << "\n//////////LoginTest Begin////////////////" << std::endl;
    AuthenticationToken token;
    generatorUserToken(principal, passwd, &token);
    SecurityManager::instance().login(token, boost::bind(loginCB, _1, _2, _3, _4));
    std::cout << "//////////LoginTest End////////////////" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
void authzCB2(bool ret, const std::string &principal, const std::string &permission){
    if(ret) std::cout << "Pass authorization: ";
    else std::cout << "Not pass authorization: ";
    std::cout << principal << "<---->" << permission << std::endl;
}

void AuthzTest(const std::string &principal, const std::string &permission){
    //PrincipalCollection pc(principal, "sqlRealm");
    SecurityManager::instance().isPermitted(principal, permission, boost::bind(authzCB2, _1, _2, _3));
}
////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
    LoadConfigTest();
    //SecurityManagerTest();
    //RedisTest();
    //WSSETest();
    //cryptTest("chenhl", "123456", 1);
    //sqlRealmTest();
    //std::string principal("chenhl");
    //std::string passwd("123456");
    //HashedCredentialsMatcherTest(principal, passwd);
    //AuthenticatorTest(principal, passwd);
    //LoginTest(principal, passwd);
    AuthzTest("zhang", "A:B:C");

    int a;
    cin >> a;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
