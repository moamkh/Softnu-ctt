#ifndef AMIDBUTILS_H
#define AMIDBUTILS_H

#include "../../../database/db_connection_pool.h"

class AmiDbUtils
{
public:
    AmiDbUtils();

private:
    DbConnectionPool* db_con_pool;
};

#endif // AMIDBUTILS_H
