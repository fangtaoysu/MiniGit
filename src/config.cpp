#include "../include/config.h"
#include <fstream>


using json = nlohmann::json;

Config::Config(const std::string& project_path)
    : config_path_(project_path + "/.mgit/config") {
    core_.worktree = project_path;
    user_.name = "";
    user_.email = "";
}

void Config::save_config() const {
    json config_object = {
        {"core", {
            "worktree", core_.worktree
        }},
        {"user", {
            {"name", user_.name},
            {"email", user_.email}
        }}
    };
    std::ofstream config_file(config_path_);
    config_file << config_object.dump(4);
}

json Config::read_config() const {
    std::ifstream config_file(config_path_);
    return json::parse(config_file);
}