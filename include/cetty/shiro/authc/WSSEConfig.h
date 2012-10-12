/*
 * WSSEConfig.h
 *
 *  Created on: 2012-9-29
 *      Author: chenhl
 */

#if !defined(CETTY_SHIRO_AUTHC_WSSECONFIG_H)
#define CETTY_SHIRO_AUTHC_WSSECONFIG_H

#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace shiro {
namespace authc {

class WSSEConfig : public cetty::config::ConfigObject {
public:
    std::string backend;
    std::string connectionString;

    std::string userTokenQuery;
    std::string userTokenUpdate;
    std::string userTokenSave;

    int nonceLength;
    int serverTimeExpire;

    WSSEConfig();
    virtual ConfigObject* create() const { return new WSSEConfig; }
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_WSSECONFIG_H)
