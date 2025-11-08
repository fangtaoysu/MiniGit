#ifndef __REMOTE_H__
#define __REMOTE_H__
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "../database/include/git_db.h"
#include "../third_party/nlohmann/json.hpp"



namespace fs = std::filesystem;
using json = nlohmann::json;

class Remote {
    public:
        Remote();
        ~Remote()=default;
        void push_commits(const std::string& commit_hash, const std::string& tree_objects, const json& commit_object);
        void push_files(const std::unordered_map<std::string, json>& file_tree_and_object_map);
        void push_commit_file_map(const std::vector<std::string>& files_path, const std::string& tree_hash);
        void create_database();

    private:
        std::unique_ptr<Database> db_;
};

#endif