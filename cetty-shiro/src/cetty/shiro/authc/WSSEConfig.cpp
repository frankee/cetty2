/*
 * WSSEConfig.cpp
 *
 *  Created on: 2012-9-29
 *      Author: chenhl
 */

#include <cetty/shiro/authc/WSSEConfig.h>
#include <cetty/shiro/authc/WSSE.h>

#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::config;

CETTY_CONFIG_ADD_DESCRIPTOR(WSSEConfig,
                            new ConfigDescriptor(
                                7,
                                CETTY_CONFIG_FIELD(WSSEConfig, backend, STRING),
                                CETTY_CONFIG_FIELD(WSSEConfig, connectionString, STRING),
                                CETTY_CONFIG_FIELD(WSSEConfig, userTokenQuery, STRING),
                                CETTY_CONFIG_FIELD(WSSEConfig, userTokenUpdate, STRING),
                                CETTY_CONFIG_FIELD(WSSEConfig, userTokenSave, STRING),
                                CETTY_CONFIG_FIELD(WSSEConfig, nonceLength, INT32),
                                CETTY_CONFIG_FIELD(WSSEConfig, serverTimeExpire, INT32)
                            ),
                            new WSSEConfig);

WSSEConfig::WSSEConfig()
    : ConfigObject("WSSEConfig"),
      userTokenQuery(WSSE::USER_TOKEN_QUERY),
      userTokenUpdate(WSSE::USER_TOKEN_UPDATE),
      userTokenSave(WSSE::USER_TOKEN_SAVE),
      nonceLength(WSSE::NONCE_LENGTH),
      serverTimeExpire(WSSE::SERVER_TIME_EXPIRE){}

}
}
}
