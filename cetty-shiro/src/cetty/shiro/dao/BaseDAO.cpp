/*
 * BaseDAO.cpp
 *
 *  Created on: 2012-9-27
 *      Author: chenhl
 */



#include <cetty/shiro/dao/BaseDAO.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace dao {

const std::string BaseDAO::backend = "sqlite3";
const std::string BaseDAO::connString = "/home/chenhl/dev/db/data/sqlite/test";

BaseDAO::BaseDAO(){
    sql.open(BaseDAO::backend, BaseDAO::connString);
}

virtual BaseDAO::~BaseDAO(){
    sql.close();
}

}
}
}
