#include "infrastructure/config/config_loader.h"
#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"

int main() {
    // Initialize logger first
    std::filesystem::path log_config_path = PathUtils::GetProjectRoot() / "config" / "log4cplus.properties";
    InitImLogger(log_config_path.string());

    minigit::infrastructure::config::ConfigLoader config_loader;
    config_loader.LoadConfig();
    const auto& cfg = config_loader.GetConfig();

    LOG_INFO("Config loaded. mysql.enable=" << (cfg.mysql.enable ? "true" : "false"));

    return 0;
}