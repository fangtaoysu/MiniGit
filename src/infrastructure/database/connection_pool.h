#pragma once

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "infrastructure/config/app_config.h"

namespace utils = minigit::shared;

namespace minigit::infrastructure::database {

class DbConnectionPool {
public:
    DbConnectionPool() = default;
    ~DbConnectionPool();

    // Prevent copying and moving
    DbConnectionPool(const DbConnectionPool&) = delete;
    DbConnectionPool& operator=(const DbConnectionPool&) = delete;

    bool Init(const utils::MySqlSettings& db_config);
    sql::Connection* GetConnection();
    void ReleaseConnection(sql::Connection* conn);

private:
    void CloseAllConnections();
    sql::Connection* CreateConnection();

    std::queue<sql::Connection*> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;

    std::string host_;
    std::string user_;
    std::string password_;
    std::string db_name_;
    unsigned int port_;
    unsigned int pool_size_;

    sql::Driver* driver_;
};

}  // namespace minigit::infrastructure::database
