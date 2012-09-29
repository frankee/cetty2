/*
 * BaseDAO.h
 *
 *  Created on: 2012-9-27
 *      Author: chenhl
 */

#ifndef BASEDAO_H_
#define BASEDAO_H_

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>

namespace cetty {
namespace shiro {
namespace dao {

class BaseDAO{
public:
    BaseDAO();
    virtual ~BaseDAO();

protected:
    soci::session &getSql()const;

private:
    static const std::string backend;
    static const std::string connString;

    soci::session sql;
};

inline
soci::session &BaseDAO::getSql() const{
    return sql;
}

}
}
}


#endif /* BASEDAO_H_ */
