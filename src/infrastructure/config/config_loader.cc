#include "infrastructure/config/config_loader.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

namespace minigit::infrastructure::config {

void ConfigLoader::LoadConfig() {
    std::string err;
    try {
        std::ifstream ifs(PathUtils::GetProjectRoot() / "config" / "config.json");
        if (!ifs.is_open()) {
            return;
        }
        nlohmann::json j;
        ifs >> j;

        if (j.contains("logging")) {
            auto jl = j["logging"];
            if (jl.contains("level")) config_.logging.level = jl["level"].get<std::string>();
        }

        if (j.contains("mysql")) {
            auto jm = j["mysql"];
            if (jm.contains("enable")) config_.mysql.enable = jm["enable"].get<bool>();
            if (jm.contains("host")) config_.mysql.host = jm["host"].get<std::string>();
            if (jm.contains("port")) config_.mysql.port = jm["port"].get<int>();
            if (jm.contains("user")) config_.mysql.user = jm["user"].get<std::string>();
            if (jm.contains("password")) config_.mysql.password = jm["password"].get<std::string>();
            if (jm.contains("database")) config_.mysql.database = jm["database"].get<std::string>();
        }

        if (j.contains("thread_pool")) {
            auto jtp = j["thread_pool"];
            if (jtp.contains("size")) config_.thread_pool.size = jtp["size"].get<int>();
        }

    } catch (const std::exception& ex) {
    }
}

const Config& ConfigLoader::GetConfig() const {
    return config_;
}

}  // namespace minigit::infrastructure::config