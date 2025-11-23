#pragma once

#include <cppconn/connection.h>

#include "infrastructure/database/connection_pool.h"

namespace infrastructure::database {

/**
 * @class DbConnectionGuard
 * @brief An RAII wrapper for managing database connections from a pool.
 *
 * This class acquires a connection from the pool upon construction and
 * automatically releases it back to the pool upon destruction, ensuring
 * that connections are always returned, even in the case of exceptions.
 */
class DbConnectionGuard {
public:
    explicit DbConnectionGuard(DbConnectionPool& pool);
    ~DbConnectionGuard();

    // Prevent copying and moving
    DbConnectionGuard(const DbConnectionGuard&) = delete;
    DbConnectionGuard& operator=(const DbConnectionGuard&) = delete;

    sql::Connection* operator->() const;
    sql::Connection* get() const;

private:
    DbConnectionPool& pool_;
    sql::Connection* connection_;
};

}  // namespace infrastructure::database
