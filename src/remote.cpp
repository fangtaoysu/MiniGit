#include "../include/remote.h"
#include <memory>
#include <iostream>



Remote::Remote() {
    std::string db_name = fs::path(Utils::get_project_path()).filename().string() + ".db";
    fs::path db_path = fs::path(Utils::get_project_path()) / db_name;
    db_ = std::make_unique<Database>(db_path.string());
}


/** 创建该项目对应的数据库及需要的表 */
void Remote::create_database() {
    db_->execute(R"(
        CREATE TABLE IF NOT EXISTS commits (
            commit_hash TEXT PRIMARY KEY,
            author TEXT NOT NULL,
            email TEXT NOT NULL,
            message TEXT NOT NULL,
            time TEXT NOT NULL,
            tree_hash TEXT NOT NULL,
            create_time DATETIME NOT NULL,
            update_time DATETIME NOT NULL 
        )
    )");

    db_->execute(R"(
        CREATE TABLE IF NOT EXISTS files (
            file_id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_path TEXT NOT NULL,
            file_hash TEXT NOT NULL,
            modified TEXT NOT NULL,
            size INTEGER NOT NULL,
            create_time DATETIME NOT NULL,
            update_time DATETIME NOT NULL 
        )
    )");

    db_->execute(R"(
        CREATE TABLE IF NOT EXISTS commit_file_map (
            file_id INTEGER NOT NULL,
            tree_hash TEXT NOT NULL,
            create_time DATETIME NOT NULL,
            update_time DATETIME NOT NULL,
            PRIMARY KEY (tree_hash, file_id),
            FOREIGN KEY (tree_hash) REFERENCES commits(tree_hash),
            FOREIGN KEY (file_id) REFERENCES files(file_id)
        )
    )");
}


void print_sql_param(const SQL_param& param) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            std::cout << "NULL";
        } else {
            std::cout << arg;
        }
    }, param);
}

/** 保存commits信息 */
void Remote::push_commits(const std::string& commit_hash, const std::string& tree_hash, const json& commit_object) {
    std::string now = Utils::get_current_time();
    std::vector<SQL_param> params = {
        commit_hash,
        commit_object["author"].dump(),
        commit_object["email"].dump(),
        commit_object["message"].dump(),
        commit_object["time"].dump(),
        tree_hash,
        now,
        now
    };

    db_->execute_with_params(
        "INSERT INTO commits (commit_hash, author, email, message, time, tree_hash, create_time, update_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
        params
    );

}


void Remote::push_files(const std::unordered_map<std::string, json>& file_tree_and_object_map) {
    std::string now = Utils::get_current_time();
    for (auto& [path, file_info] : file_tree_and_object_map) {
        std::vector<SQL_param> params = {
            path,
            file_info["hash"].dump(),
            file_info["modified"].dump(),
            file_info["size"].dump(),
            now,
            now
        };
    
        db_->execute_with_params(
            "INSERT INTO files (file_path, file_hash, modified, size, create_time, update_time) VALUES (?, ?, ?, ?, ?, ?)",
            params
        );
    }
}


void Remote::push_commit_file_map(const std::vector<std::string>& files_path, const std::string& tree_hash) {
    std::string now = Utils::get_current_time();

    for (auto& file_path : files_path) {
        // 先根据文件路径查出file_id
        auto results = db_->query_with_params(
            "SELECT file_id FROM files WHERE file_path == (?)",
            {file_path}
        );

        int file_id;
        try {
            file_id = std::stoi(results[0]["file_id"]);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid file_id format for file: " + file_path);
        }
        std::vector<SQL_param> params = {
            file_id,
            tree_hash,
            now,
            now
        };

        db_->execute_with_params(
            "INSERT INTO commit_file_map (file_id, tree_hash, create_time, update_time) VALUES (?, ?, ?, ?)",
            params
        );
    }
    
}