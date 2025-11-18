#include "infrastructure/database/connection_pool.h"

#include <sstream>

#include "infrastructure/logging/logger.h"


namespace infrastructure::database {

DbConnectionPool::~DbConnectionPool() {
    CloseAllConnections();
}

bool DbConnectionPool::Init(const MySqlSettings& db_config) {
    if (!db_config.enable) {
        LOG_WARN("MySQL is disabled in the configuration.");
        return false;
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
                pool_.push(conn);
            } else {
                LOG_ERROR("Failed to create initial database connections.");
                CloseAllConnections();
                return false;
            }
        }
        LOG_INFO("Database connection pool initialized successfully with {} connections." << pool_.size());
        return true;
    } catch (const sql::SQLException& e) {
        std::stringstream error_msg;
        error_msg << "SQLException during pool initialization: " << e.what()
                  << " (MySQL error code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << ")";
        LOG_ERROR(error_msg.str());
        return false;
    }
}

sql::Connection* DbConnectionPool::GetConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]() { return !pool_.empty(); });

    sql::Connection* conn = pool_.front();
    pool_.pop();
    return conn;
}

void DbConnectionPool::ReleaseConnection(sql::Connection* conn) {
    if (conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        pool_.push(conn);
        lock.unlock();
        cond_.notify_one();
    }
}

void DbConnectionPool::CloseAllConnections() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!pool_.empty()) {
        sql::Connection* conn = pool_.front();
        pool_.pop();
        delete conn;
    }
    LOG_INFO("All database connections have been closed.");
}

sql::Connection* DbConnectionPool::CreateConnection() {
    try {
        sql::ConnectOptionsMap connection_properties;
        connection_properties["hostName"] = host_;
        connection_properties["userName"] = user_;
        connection_properties["password"] = password_;
        connection_properties["schema"] = db_name_;
        connection_properties["port"] = static_cast<int>(port_);

        sql::Connection* conn = driver_->connect(connection_properties);
        return conn;
    } catch (const sql::SQLException& e) {
        std::stringstream error_msg;
        error_msg << "Failed to create a database connection: " << e.what()
                  << " (MySQL error code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << ")";
        LOG_ERROR(error_msg.str());
        return nullptr;
    }
}

} // namespace infrastructure::database