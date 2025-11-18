#include "infrastructure/config/config_loader.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"

namespace minigit::infrastructure::config {

void ConfigLoader::LoadConfig() {
    try {
        const std::string config_path = PathUtils::GetProjectRoot() / "config" / "config.json";
        std::ifstream ifs(config_path);
        if (!ifs.is_open()) {
            LOG_ERROR("cannot open: " + config_path);
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
            if (jm.contains("db_name")) config_.mysql.db_name = jm["db_name"].get<std::string>();
            if (jm.contains("pool_size")) config_.mysql.pool_size = jm["pool_size"].get<int>();
        }

        if (j.contains("thread_pool")) {
            auto jtp = j["thread_pool"];
            if (jtp.contains("size")) config_.thread_pool.size = jtp["size"].get<int>();
        }

        LOG_INFO("Config loaded: " + config_path);
    } catch (const std::exception& ex) {
        LOG_ERROR("Config load error: " + std::string{ex.what()});
    }
}

const Config& ConfigLoader::GetConfig() const {
    return config_;
}

}  // namespace minigit::infrastructure::config