#include <iostream>

#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionPtr.h>
#include <cetty/shiro/session/RedisSessionDAO.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/util/SecurityUtils.h>

#include <cetty/config/ConfigCenter.h>
#include <cetty/redis/RedisClient.h>
#include <cetty/redis/builder/RedisClientBuilder.h>
#include <gtest/gtest.h>
#include <cetty/util/StringPiece.h>

using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;
using namespace cetty::shiro::util;
using namespace cetty::shiro;
using namespace cetty::config;
using namespace cetty::util;
using namespace testing;
using namespace std;

using namespace cetty::redis;
using namespace cetty::redis::builder;


TEST(ConfigTest, RedisConfig){
    EXPECT_EQ(true, ConfigCenter::instance().loadFromFile("shiro.conf"));
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

void createSessionCb(int ret, SessionPtr session){
    EXPECT_EQ(0, ret);
    std::string json;
    session->toJson(&json);
    std::cout << json << std::endl;
}

void getSessionCb(int ret, SessionPtr session){
    EXPECT_EQ(0, ret);
    EXPECT_STREQ("value1", session->getAttribute("key1").c_str());
    EXPECT_STREQ("123", session->getAttribute("key2").c_str());
    std::cout << session->getAttribute("key1") << std::endl;
    std::cout << session->getAttribute("key2") << std::endl;
}

TEST(SessionTest, RedisSessionDAO){
    RedisSessionDAO dao;
    SessionPtr session = new Session();
    session->setAttribute("key1", "value1");
    session->setAttribute("key2", "123");
    dao.create(session, boost::bind(createSessionCb, _1, _2));

    // todo hava segmentation fault
    //dao.readSession(session->getId(), boost::bind(getSessionCb, _1, _2));
}

void getSession(int ret, const StringPiece &str){
    std::cout << ret << std::endl;
    std::cout << str.c_str() << std::endl;
}

void SMGetSession(const SessionPtr &session){
    EXPECT_STREQ("test1", session->getAttribute("sm1").c_str());
    EXPECT_STREQ("test2", session->getAttribute("sm2").c_str());
}

void SMStartSession(const SessionPtr &session){
    session->setAttribute("sm1", "test1");
    session->setAttribute("sm2", "test2");
    SecurityManager::instance().getSessionManager()->getSession(session->getId(), boost::bind(SMGetSession, _1));
}


TEST(SessionManagerTest, getSession){
    SecurityManager::instance().getSessionManager()->start("localhost", boost::bind(SMStartSession, _1));
}

int main(int argc, char **argv){

    RedisClientBuilder builder;
    builder.addConnection("127.0.0.1", 6379);
    RedisClient *redis = new RedisClient(builder.build());
    redis->get("name", boost::bind(getSession, _1, _2));

    //::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
    return 0;
}
