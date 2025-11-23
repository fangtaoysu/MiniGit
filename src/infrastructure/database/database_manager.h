#pragma once

#include <cppconn/connection.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "infrastructure/config/app_config.h"

namespace infrastructure::database {

class DbConnectionPool;
class DbConnectionGuard;

using QueryResult = std::map<std::string, std::string>;

class DbManager {
public:
    static DbManager& GetInstance();

    bool Initialize(const MySqlSettings& config);

    std::vector<QueryResult> Query(const std::string& sql,
                                   const std::vector<std::string>& params = {});
    int64_t Execute(const std::string& sql,
                    const std::vector<std::string>& params = {});

    void BeginTransaction();
    void CommitTransaction();
    void RollbackTransaction();

private:
    DbManager() = default;
    ~DbManager() = default;
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    // Helper to get the right connection (transactional or from pool)
    std::pair<sql::Connection*, std::unique_ptr<DbConnectionGuard>>
    GetConnectionForOperation();

    std::unique_ptr<DbConnectionPool> pool_;
    static thread_local sql::Connection* transaction_connection_;
};

}  // namespace infrastructure::database
