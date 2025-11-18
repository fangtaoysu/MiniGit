#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <string>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>

namespace sql {
    class Connection;
}

class MySQLConnectionPool {
public:
    static MySQLConnectionPool& GetInstance();

    bool Init();
    std::shared_ptr<sql::Connection> GetConnection();

    MySQLConnectionPool(const MySQLConnectionPool&) = delete;
    MySQLConnectionPool& operator=(const MySQLConnectionPool&) = delete;
    MySQLConnectionPool(MySQLConnectionPool&&) = delete;
    MySQLConnectionPool& operator=(MySQLConnectionPool&&) = delete;

private:
    MySQLConnectionPool() = default;
    ~MySQLConnectionPool();

    void ReleaseConnection(sql::Connection* conn);
    sql::Connection* CreateConnection();

    bool initialized_ = false;
    std::string host_, user_, password_, db_name_;
    unsigned int port_;
    unsigned int pool_size_;

    std::queue<sql::Connection*> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
    sql::Driver* driver_;
};
