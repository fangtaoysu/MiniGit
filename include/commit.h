#ifndef __COMMIT_H__
#define __COMMIT_H__

#include "utils.h"
#include "../thrid_party/nlohmann/json.hpp"
#include "file_system.h"
#include "object_db.h"
#include "index.h"
#include "config.h"

using json = nlohmann::json;


class Commit {
    public:
        Commit()=default;
        ~Commit() {}
        void run(const std::string& msg);
        static std::vector<std::string> read_tree_object();
        static const std::string get_commit_hash();
        static std::string get_tree_hash();
        
    private:
        bool is_index_changed(const json& entries) const;
        void save_to_objects(const std::string& father_ref, const std::vector<std::string>& tree_objects, const std::string& msg);
        void save_to_HEAD(const std::string& father_ref, const std::string& msg);

        const std::string project_path_ = Utils::get_project_path();
        std::string current_ref_;
        std::string current_timestamp_;
};

#endif