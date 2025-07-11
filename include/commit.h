#ifndef __COMMIT_H__
#define __COMMIT_H__

#include <sstream>
#include <vector>
#include "../thrid_party/nlohmann/json.hpp"


using json = nlohmann::json;
class Commit {
    public:
        Commit(const std::string& project_path);
        ~Commit() {}
        void run(const std::string& msg);
        
    private:
        bool is_index_changed(const json& entries) const;
        const std::string& project_path_;
        std::string current_ref_;
        std::string current_timestamp_;

        void save_to_objects(const std::string& father_ref, const std::string& msg);
        void save_to_HEAD(const std::string& father_ref, const std::string& msg);
        const std::string get_father_ref();
};

#endif