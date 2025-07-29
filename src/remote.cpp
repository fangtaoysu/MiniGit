#include "../include/remote.h"
#include <memory>



/** 把commit对象、tree对象和tree对象对应的文件等信息写入表中 */
void Remote::push(const std::string& commit_hash, const std::vector<std::string>& tree_objects, const std::unordered_map<std::string, std::string>& tree_and_hash_map) {
    // 获取commit对象，并保存在commits表中
    
}

/** 创建该项目对应的数据库及需要的表 */
void Remote::create_database(const std::string& project_path) {
    std::string db_name = fs::path(project_path).filename().string() + ".db";
    fs::path db_path = fs::path(project_path) / db_name;

    std::unique_ptr<Database> db = std::make_unique<Database>(db_path.string());
    db->execute(R"(
        CREATE TABLE IF NOT EXISTS commits (
            commit_hash TEXT PRIMARY KEY,
            author TEXT NOT NULL,
            message TEXT NOT NULL,
            time TEXT NOT NULL,
            tree_object TEXT NOT NULL,
            create_time DATETIME NOT NULL,
            update_time DATETIME NOT NULL 
        )
    )");

    db->execute(R"(
        CREATE TABLE IF NOT EXISTS files (
            file_id INTEGER PRIMARY KEY,
            file_path TEXT NOT NULL,
            file_hash TEXT NOT NULL,
            modified TEXT NOT NULL,
            size INTEGER NOT NULL,
            content INTEGER,
            create_time DATETIME NOT NULL,
            update_time DATETIME NOT NULL 
        )
    )");

    db->execute(R"(
        CREATE TABLE IF NOT EXISTS commit_file_map (
            commit_hash TEXT NOT NULL,
            file_hash TEXT NOT NULL,
            file_path TEXT NOT NULL,
            modified_time DATETIME NOT NULL,
            PRIMARY KEY (commit_hash, file_path),
            FOREIGN KEY (commit_hash) REFERENCES commits(commit_hash),
            FOREIGN KEY (file_hash) REFERENCES file_objects(file_hash)
        )
    )");
}