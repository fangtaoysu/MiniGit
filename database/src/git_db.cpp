#include "../include/git_db.h"
#include <string>



Database::Database(const std::string& filename) {
    if (sqlite3_open(filename.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
    }
}


Database::~Database() {
    sqlite3_close(db_);
}


void Database::check_error(int rc, const std::string& context) {
    if (rc != SQLITE_OK) {
        throw std::runtime_error(context + ": " + sqlite3_errmsg(db_));
    }
}


void Database::execute(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string err(err_msg);
        sqlite3_free(err_msg);
        throw std::runtime_error("SQL error: " + err);
    }
}

std::vector<std::unordered_map<std::string, std::string>> Database::Query(const std::string& sql) {
    std::vector<std::unordered_map<std::string, std::string>> res;
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    check_error(rc, "Failed to prepare statement");

    int col_count = sqlite3_column_count(stmt);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::unordered_map<std::string, std::string> row;
        for (int i = 0; i < col_count; ++i) {
            const char* col_name = sqlite3_column_name(stmt, i);
            const char* col_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row[col_name] = col_value ? col_value : "NULL";
        }
        res.push_back(row);
    }

    if (rc != SQLITE_DONE) {
        check_error(rc, "Error during query execution");
    }

    sqlite3_finalize(stmt);
    return res;
}


void Database::execute_with_params(const std::string& sql, const std::vector<SQL_param>& params) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    check_error(rc, "Failed to prepare statement");

    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        int bind_idx = i + 1;
        if (std::holds_alternative<int>(param)) {
            rc = sqlite3_bind_int(stmt, bind_idx, std::get<int>(param));
        } else if (std::holds_alternative<double>(param)) {
            rc = sqlite3_bind_double(stmt, bind_idx, std::get<double>(param));
        } else if (std::holds_alternative<std::string>(param)) {
            const auto& str = std::get<std::string>(param);
            rc = sqlite3_bind_text(stmt, bind_idx, str.c_str(), str.length(), SQLITE_TRANSIENT);
        } else if (std::holds_alternative<std::nullptr_t>(param)) {
            rc = sqlite3_bind_null(stmt, bind_idx);
        } 
        check_error(rc, "Failed to bind parameter at index" + std::to_string(i));
    }

    // 执行
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        check_error(rc, "Failed to execute statement");
    } 

    sqlite3_finalize(stmt);
}


std::vector<std::unordered_map<std::string, std::string>> Database::query_with_params(const std::string& sql, const std::vector<SQL_param>& params) {
    sqlite3_stmt* stmt;
    std::vector<std::unordered_map<std::string, std::string>> res;

    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    check_error(rc, "Failed to prepare statement");

    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        int bind_idx = i + 1;
        if (std::holds_alternative<int>(param)) {
            rc = sqlite3_bind_int(stmt, bind_idx, std::get<int>(param));
        } else if (std::holds_alternative<double>(param)) {
            rc = sqlite3_bind_double(stmt, bind_idx, std::get<double>(param));
        } else if (std::holds_alternative<std::string>(param)) {
            const auto& str = std::get<std::string>(param);
            rc = sqlite3_bind_text(stmt, bind_idx, str.c_str(), str.length(), SQLITE_TRANSIENT);
        } else if (std::holds_alternative<std::nullptr_t>(param)) {
            rc = sqlite3_bind_null(stmt, bind_idx);
        } 
        check_error(rc, "Failed to bind parameter at index" + std::to_string(i));
    }

    int col_count = sqlite3_column_count(stmt);
    std::vector<std::string> col_names;
    for (int i = 0; i < col_count; ++i) {
        col_names.emplace_back(sqlite3_column_name(stmt, i));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::unordered_map<std::string, std::string> row;
        for (int i = 0; i < col_count; ++i) {
            const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row[col_names[i]] = val ? val : "NULL";
        }
        res.push_back(row);
    }

    if (rc != SQLITE_DONE) {
        check_error(rc, "Failed during query execution");
    }

    sqlite3_finalize(stmt);
    return res;
}
