#include "infrastructure/database/database_manager.h"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>

#include <sstream>

#include "infrastructure/database/connection_pool.h"
#include "infrastructure/database/db_connection_guard.h"
#include "infrastructure/logging/logger.h"

namespace infrastructure::database {

// Initialize the static thread_local variable
thread_local sql::Connection* DbManager::transaction_connection_ = nullptr;

DbManager& DbManager::GetInstance() {
    static DbManager instance;
    return instance;
}

bool DbManager::Initialize(const MySqlSettings& config) {
    if (!config.enable) {
        LOG_WARN("Database Manager initialization skipped: MySQL is disabled.");
        return true;  // Return true as it's a valid state
    }
    pool_ = std::make_unique<DbConnectionPool>();
    return pool_->Init(config);
}

std::vector<QueryResult> DbManager::Query(
    const std::string& sql, const std::vector<std::string>& params) {
    if (!pool_) {
        LOG_ERROR("Query failed: DbManager is not initialized.");
        return {};
    }

    try {
        auto [conn, guard] = GetConnectionForOperation();
        if (!conn) {
            LOG_ERROR(
                "Query failed: Could not get a valid database connection.");
            return {};
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(sql));

        for (size_t i = 0; i < params.size(); ++i) {
            pstmt->setString(i + 1, params[i]);
        }

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::vector<QueryResult> results;
        sql::ResultSetMetaData* meta = res->getMetaData();
        int num_columns = meta->getColumnCount();

        while (res->next()) {
            QueryResult row;
            for (int i = 1; i <= num_columns; ++i) {
                row[meta->getColumnLabel(i)] = res->getString(i);
            }
            results.push_back(row);
        }
        return results;
    } catch (const sql::SQLException& e) {
        std::stringstream error_msg;
        error_msg << "SQL Query failed: " << e.what()
                  << " (Error code: " << e.getErrorCode() << ")";
        LOG_ERROR(error_msg.str());
        return {};
    }
}

int64_t DbManager::Execute(const std::string& sql,
                           const std::vector<std::string>& params) {
    if (!pool_) {
        LOG_ERROR("Execute failed: DbManager is not initialized.");
        return -1;
    }

    try {
        auto [conn, guard] = GetConnectionForOperation();
        if (!conn) {
            LOG_ERROR(
                "Execute failed: Could not get a valid database connection.");
            return -1;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(sql));

        for (size_t i = 0; i < params.size(); ++i) {
            pstmt->setString(i + 1, params[i]);
        }

        int64_t affected_rows = pstmt->executeUpdate();
        return affected_rows;
    } catch (const sql::SQLException& e) {
        std::stringstream error_msg;
        error_msg << "SQL Execute failed: " << e.what()
                  << " (Error code: " << e.getErrorCode() << ")";
        LOG_ERROR(error_msg.str());
        return -1;
    }
}

void DbManager::BeginTransaction() {
    if (transaction_connection_) {
        LOG_WARN(
            "BeginTransaction called while a transaction is already active.");
        return;
    }
    if (!pool_) {
        LOG_ERROR("BeginTransaction failed: DbManager is not initialized.");
        return;
    }
    transaction_connection_ = pool_->GetConnection();
    transaction_connection_->setAutoCommit(false);
    LOG_DEBUG("Transaction started.");
}

void DbManager::CommitTransaction() {
    if (!transaction_connection_) {
        LOG_WARN("CommitTransaction called without an active transaction.");
        return;
    }
    try {
        transaction_connection_->commit();
        LOG_DEBUG("Transaction committed.");
    } catch (const sql::SQLException& e) {
        std::stringstream error_msg;
        error_msg << "Transaction commit failed: " << e.what()
                  << " (Error code: " << e.getErrorCode() << ")";
        LOG_ERROR(error_msg.str());
    }
    transaction_connection_->setAutoCommit(true);
    pool_->ReleaseConnection(transaction_connection_);
    transaction_connection_ = nullptr;
}

void DbManager::RollbackTransaction() {
    if (!transaction_connection_) {
        LOG_WARN("RollbackTransaction called without an active transaction.");
        return;
    }
    try {
        transaction_connection_->rollback();
        LOG_DEBUG("Transaction rolled back.");
    } catch (const sql::SQLException& e) {
        std::stringstream error_msg;
        error_msg << "Transaction rollback failed: " << e.what()
                  << " (Error code: " << e.getErrorCode() << ")";
        LOG_ERROR(error_msg.str());
    }
    transaction_connection_->setAutoCommit(true);
    pool_->ReleaseConnection(transaction_connection_);
    transaction_connection_ = nullptr;
}

std::pair<sql::Connection*, std::unique_ptr<DbConnectionGuard>>
DbManager::GetConnectionForOperation() {
    if (transaction_connection_) {
        // If in a transaction, use the transaction's connection.
        // The guard is null because we don't want to release the connection.
        return {transaction_connection_, nullptr};
    } else {
        // If not in a transaction, get a connection from the pool.
        // The guard will manage the connection's lifecycle.
        auto guard = std::make_unique<DbConnectionGuard>(*pool_);
        sql::Connection* conn = guard->get();
        return {conn, std::move(guard)};
    }
}

}  // namespace infrastructure::database