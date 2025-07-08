#ifndef __COMMIT_H__
#define __COMMIT_H__

#include <sstream>


class Commit {
    public:
        Commit(const std::string& project_path);
        ~Commit() {}
        const std::string run(const std::string& msg, std::string father_ref);

    private:
        const std::string& project_path_;
        std::string current_ref_;
        std::string current_timestamp_;

        void save_to_objects(const std::string& father_ref, const std::string& msg);
        void save_to_HEAD(const std::string& father_ref, const std::string& msg);
};

#endif