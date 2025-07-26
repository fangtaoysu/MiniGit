#ifndef __COMMIT_H__
#define __COMMIT_H__

#include "../thrid_party/nlohmann/json.hpp"
using json = nlohmann::json;


class Commit {
    public:
        Commit(const std::string& project_path);
        ~Commit() {}
        void run(const std::string& msg);
        static std::vector<std::string> read_tree_object(const std::string& project_path);
        static const std::string read_commit_hash(const std::string& project_path);
        
    private:
        bool is_index_changed(const json& entries) const;
        void save_to_objects(const std::string& father_ref, const std::vector<std::string>& tree_objects, const std::string& msg);
        void save_to_HEAD(const std::string& father_ref, const std::string& msg);

        const std::string& project_path_;
        std::string current_ref_;
        std::string current_timestamp_;
};

#endif