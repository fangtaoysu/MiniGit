#ifndef __REMOTE_H__
#define __REMOTE_H__
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "../database/include/git_db.h"



namespace fs = std::filesystem;

class Remote {
    public:
        Remote()=default;
        ~Remote()=default;
        void push(const std::string& commit_object, const std::vector<std::string>& tree_objects, const std::unordered_map<std::string, std::string>& tree_and_hash_map);
        void create_database(const std::string& project_path);

};

#endif