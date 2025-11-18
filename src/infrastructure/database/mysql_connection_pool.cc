#include "mysql_connection_pool.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "infrastructure/logging/logger.h"


MySQLConnectionPool& MySQLConnectionPool::GetInstance() {
    static MySQLConnectionPool instance;
    return instance;
}

bool MySQLConnectionPool::Init(const MySqlSettings& db_config) {
    if (initialized_) {
        return true;
    }

    if (!db_config.enable) {
        LOG_INFO("MySQL is disabled in config, skipping connection pool initialization.");
        initialized_ = true; // Mark as initialized to avoid re-attempts
        return true;
    }

    try {
        driver_ = get_driver_instance();
        host_ = db_config.host;
        user_ = db_config.user;
        password_ = db_config.password;
        db_name_ = db_config.db_name;
        port_ = db_config.port;
        pool_size_ = db_config.pool_size;

        for (unsigned int i = 0; i < pool_size_; ++i) {
            sql::Connection* conn = CreateConnection();
            if (conn) {
                connections_.push(conn);
            } else {
                LOG_WARN("Failed to create initial MySQL connection " << (i + 1) << " of " << pool_size_);
            }
        }

        if (connections_.empty() && pool_size_ > 0) {
            LOG_ERROR("Failed to initialize any MySQL connections for the pool.");
            return false;
        }

        initialized_ = true;
        LOG_INFO("MySQL connection pool initialized successfully with " << connections_.size() << " connections.");
        return true;

    } catch (const sql::SQLException& e) {
        LOG_ERROR("Failed to initialize MySQL connection pool: " << e.what());
        return false;
    } catch (const std::exception& e) {
        LOG_ERROR("An unexpected error occurred during MySQL pool initialization: " << e.what());
        return false;
    }
}

std::shared_ptr<sql::Connection> MySQLConnectionPool::GetConnection() {
    if (!initialized_ || pool_size_ == 0) {
        LOG_WARN("Attempted to get a connection from an uninitialized or disabled pool.");
        return nullptr;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] { return !connections_.empty(); });

    if (connections_.empty()) {
        LOG_ERROR("Failed to get a connection: pool is empty after waiting.");
        return nullptr;
    }

    sql::Connection* conn = connections_.front();
    connections_.pop();

    return std::shared_ptr<sql::Connection>(conn, [this](sql::Connection* c) {
        ReleaseConnection(c);
    });
}

MySQLConnectionPool::~MySQLConnectionPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!connections_.empty()) {
        sql::Connection* conn = connections_.front();
        connections_.pop();
        try {
            if (conn) {
                conn->close();
                delete conn;
            }
        } catch (const sql::SQLException& e) {
            LOG_ERROR("Error closing connection on pool destruction: " << e.what());
        } catch (const std::exception& e) {
            LOG_ERROR("Unexpected error on pool destruction: " << e.what());
        }
    }
}

void MySQLConnectionPool::ReleaseConnection(sql::Connection* conn) {
    if (!conn) return;

    try {
        // A simple 'ping' to check if the connection is still alive.
        if (conn->isClosed() || !conn->isValid()) {
            LOG_WARN("Released connection is closed or invalid. Creating a new one.");
            delete conn;
            conn = CreateConnection();
        }
    } catch (const sql::SQLException& e) {
        LOG_ERROR("Error checking connection status, discarding and creating a new one: " << e.what());
        delete conn;
        conn = CreateConnection();
    }

    if (conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.push(conn);
        cond_.notify_one();
    } else {
        LOG_ERROR("Failed to create a replacement connection.");
    }
}

sql::Connection* MySQLConnectionPool::CreateConnection() {
    try {
        sql::ConnectOptionsMap connection_properties;
        connection_properties["hostName"] = host_;
        connection_properties["userName"] = user_;
        connection_properties["password"] = password_;
        connection_properties["schema"] = db_name_;
        connection_properties["port"] = static_cast<int>(port_);

        return driver_->connect(connection_properties);
    } catch (const sql::SQLException& e) {
        LOG_ERROR("Failed to create MySQL connection: " << e.what());
        return nullptr;
    }
}