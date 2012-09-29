/*
 * WSSEDAO.h
 *
 *  Created on: 2012-9-27
 *      Author: chenhl
 */

#ifndef WSSEDAO_H_
#define WSSEDAO_H_

#include <string>
#include <soci.h>
#include <sqlite3/soci-sqlite3.h>

#include <cetty/shiro/dao/BaseDAO.h>
#include <cetty/shiro/authc/WSSE.h>

namespace cetty {
namespace shiro {
namespace dao {

using namespace cetty::shiro::authc;

class WSSEDAO : public BaseDAO{
public:
    WSSEDAO(){};
    virtual ~WSSEDAO(){}

    void queryLoginSecret(const LoginSecret &oldLs, LoginSecret *newLs);
    void saveLoginSecret(const LoginSecret &ls);

protected:
    // hook method
    virtual void doQuery(const LoginSecret &oldLs, LoginSecret *newLs);
    virtual void doSave(const LoginSecret &ls);
};

}
}
}


#endif /* WSSEDAO_H_ */
