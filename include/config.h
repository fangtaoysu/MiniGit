#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <string>
#include "../thrid_party/nlohmann/json.hpp"

struct Core {
    std::string worktree;
};

struct User {
    std::string name;
    std::string email;
};


class Config {
    using json = nlohmann::json;
    public:
        Config(const std::string& project_path);
        void save_config() const;
        json read_config() const;
        User user_;
         
    private:
        Core core_;
        const std::string config_path_;
};

#endif