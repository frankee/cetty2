#if !defined(CETTY_SHIRO_SESSION_REDISSESSIONDAO_H)
#define CETTY_SHIRO_SESSION_REDISSESSIONDAO_H

#include <cetty/redis/RedisClient.h>
#include <cetty/shiro/session/SessionDAO.h>
#include <cetty/shiro/session/RedisConfig.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace cetty::redis;

class RedisSessionDAO : public SessionDAO {
public:
    RedisSessionDAO();
    virtual ~RedisSessionDAO();

    virtual void readSession(const std::string& sessionId, const SessionCallback& callback);
    virtual void create(const SessionPtr& session, const SessionCallback& callback);

    virtual void update(const SessionPtr& session, const SessionCallback& callback);
    virtual void remove(const SessionPtr& session, const SessionCallback& callback);

private:
    void getSessionCallback(int result,
                            const StringPiece& data,
                            const SessionCallback& callback);

private:
    RedisConfig config;
    RedisClient* redis;
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_SESSION_REDISSESSIONDAO_H)

// Local Variables:
// mode: c++
// End:
