#include "infrastructure/database/db_connection_guard.h"

namespace infrastructure::database {

DbConnectionGuard::DbConnectionGuard(DbConnectionPool& pool)
    : pool_(pool), connection_(pool.GetConnection()) {}

DbConnectionGuard::~DbConnectionGuard() {
    if (connection_) {
        pool_.ReleaseConnection(connection_);
    }
}

sql::Connection* DbConnectionGuard::operator->() const {
    return connection_;
}

sql::Connection* DbConnectionGuard::get() const {
    return connection_;
}

} // namespace infrastructure::database